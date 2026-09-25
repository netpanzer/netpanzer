import { test, expect } from '@playwright/test';

// Observe the real protocol, including its changing XOR header key, so the
// test distinguishes issuing an order from unrelated ping or bot traffic.
function observePackets(keyMessage, onMessage) {
  let pending = Buffer.alloc(0), key = 0;
  function visit(message) {
    if (message[0] === 0) {
      for (let offset = 2; offset + 2 <= message.length;) {
        const length = message.readUInt16LE(offset);
        if (length < 2 || offset + 2 + length > message.length) throw new Error('Invalid multi-packet');
        visit(message.subarray(offset + 2, offset + 2 + length));
        offset += 2 + length;
      }
    } else onMessage(message);
  }
  return ({ payload }) => {
    pending = Buffer.concat([pending, Buffer.from(payload)]);
    while (pending.length >= 2) {
      const length = pending.readUInt16LE(0);
      if (pending.length < length + 2) break;
      const message = Buffer.from(pending.subarray(2, length + 2));
      pending = pending.subarray(length + 2);
      message[0] ^= key;
      message[1] ^= key;
      if (message[0] === 1 && message[1] === keyMessage) key = message[2];
      visit(message);
    }
  };
}

test('loads the real game, connects, renders and responds to input', async ({ page }, testInfo) => {
  const errors = [], messages = [];
  let playerID, spawned = 0, styles = 0, flags = 0, orders = 0;
  let received = 0, sent = 0;
  page.on('pageerror', error => errors.push(error.message));
  page.on('console', msg => messages.push(`${msg.type()}: ${msg.text()}`));
  page.on('websocket', socket => {
    socket.on('framereceived', () => received++);
    socket.on('framesent', () => sent++);
    socket.on('framereceived', observePackets(6, message => {
      if (message[0] === 3 && message[1] === 0) playerID = message[2];
      if (message[0] === 4 && message[1] === 3 && message[2] === playerID) spawned++;
    }));
    socket.on('framesent', observePackets(7, message => {
      if (message[0] === 3 && message[1] === 8) styles++;
      if (message[0] === 3 && message[1] === 2) flags++;
      if (message[0] === 6 && message[1] === 0 && message[6] === 0) orders++;
    }));
  });
  await page.goto('/');
  await expect(page.getByRole('button', { name: 'Play NetPanzer' })).toBeEnabled({ timeout: 60000 });
  await page.getByRole('button', { name: 'Play NetPanzer' }).click();
  const canvas = page.locator('canvas');
  let rect = await canvas.boundingBox();
  const point = (x, y) => [rect.x + x * rect.width / 800, rect.y + y * rect.height / 600];
  await expect(page.locator('#status')).toHaveAttribute('data-state', 'menu');
  await page.waitForTimeout(300);
  expect(received).toBe(0);  // Opening the game must not auto-join.
  await canvas.screenshot({ path: testInfo.outputPath('main-menu.png') });
  await page.mouse.click(...point(235, 63));  // Options, where Host used to be.
  await page.waitForTimeout(200);
  await canvas.screenshot({ path: testInfo.outputPath('options.png') });
  const serversResponse = page.waitForResponse(response => response.url().endsWith('/servers'));
  await page.mouse.click(...point(160, 63));  // Join.
  const servers = await (await serversResponse).json();
  await expect(page.locator('#status')).toContainText('Choose a server, enter');
  expect(servers.length).toBeGreaterThan(0);
  await page.mouse.click(...point(240, 324));  // Select the first real server.
  await canvas.screenshot({ path: testInfo.outputPath('server-list.png') });
  const connection = page.waitForEvent('websocket');
  await page.mouse.click(...point(688, 563));  // In-game Play button.
  expect(new URL((await connection).url()).searchParams.get('server')).toBe(servers[0].id);
  await expect.poll(() => received, { timeout: 40000 }).toBeGreaterThan(20);
  await expect.poll(() => sent, { timeout: 40000 }).toBeGreaterThan(5);
  await expect(page.locator('#status')).toHaveAttribute('data-state', 'playing', { timeout: 40000 });
  await page.getByRole('button', { name: 'Fullscreen' }).click();
  await expect.poll(() => page.evaluate(() => !!document.fullscreenElement)).toBe(true);
  rect = await canvas.boundingBox();
  // Intentionally quick clicks: both edges may arrive within one RAF frame.
  await page.mouse.click(...point(400, 134));
  await expect.poll(() => styles).toBeGreaterThan(0);
  await page.mouse.click(...point(175, 100));
  await expect.poll(() => flags).toBeGreaterThan(0);
  await expect.poll(() => spawned, { timeout: 15000 }).toBeGreaterThan(0);
  await page.evaluate(() => document.exitFullscreen());
  await expect.poll(() => page.evaluate(() => !!document.fullscreenElement)).toBe(false);
  await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
  rect = await canvas.boundingBox();
  const dimensions = await canvas.evaluate(c => [c.width, c.height]);
  await page.keyboard.press('Alt+Enter');
  await page.keyboard.press('F9');
  await page.waitForTimeout(150);
  expect(await canvas.evaluate(c => [c.width, c.height])).toEqual(dimensions);
  expect(await page.evaluate(() => !!document.fullscreenElement)).toBe(false);
  await page.mouse.move(...point(400, 300));
  await page.waitForTimeout(1000);
  await canvas.screenshot({ path: testInfo.outputPath('spawn.png') });
  await page.mouse.move(...point(170, 50));
  await page.mouse.down();
  await page.mouse.move(...point(790, 420), { steps: 20 });
  await page.waitForTimeout(100);
  await page.mouse.up();
  await page.waitForTimeout(100);
  // Try several ground locations, since the server rotates spawn points.
  for (const x of [300, 400, 500, 600]) {
    await page.mouse.click(...point(x, 300));
    await page.waitForTimeout(250);
    if (orders > 0) break;
  }
  expect(orders, 'selection must produce a move order on the actual network').toBeGreaterThan(0);
  await page.waitForTimeout(1500);
  const before = await canvas.screenshot();
  await canvas.focus();
  await page.keyboard.down('ArrowRight');
  await page.waitForTimeout(700);
  await page.keyboard.up('ArrowRight');
  const after = await canvas.screenshot();
  expect(before.equals(after)).toBe(false);
  await page.screenshot({ path: testInfo.outputPath('battlefield.png'), fullPage: true });
  // Resign back to the usual menu and reconnect in the same Wasm instance.
  await page.keyboard.press('Escape');
  await page.waitForTimeout(200);
  await page.mouse.click(...point(245, 63));
  await page.waitForTimeout(200);
  await canvas.screenshot({ path: testInfo.outputPath('resign.png') });
  await page.mouse.click(...point(320, 340));
  await expect(page.locator('#status')).toHaveAttribute('data-state', 'menu');
  await page.mouse.click(...point(160, 63));
  await expect(page.locator('#status')).toContainText('Choose a server, enter');
  await page.mouse.click(...point(240, 324));
  const reconnect = page.waitForEvent('websocket');
  await page.mouse.click(...point(688, 563));
  await reconnect;
  await expect(page.locator('#status')).toHaveAttribute('data-state', 'playing', { timeout: 40000 });
  await testInfo.attach('browser-log', { body: messages.join('\n'), contentType: 'text/plain' });
  expect(errors).toEqual([]);
  await expect(page.locator('#status')).not.toContainText('Unable');
});

test('server-list failures can be retried and unapproved addresses stay in the menu', async ({ page }, testInfo) => {
  let connections = 0, fail = true;
  const errors = [];
  page.on('websocket', () => connections++);
  page.on('pageerror', error => errors.push(error.message));
  await page.route('**/servers', route => fail ? route.fulfill({ status: 503, body: '' }) : route.continue());
  await page.goto('/');
  await page.getByRole('button', { name: 'Play NetPanzer' }).click({ timeout: 60000 });
  const rect = await page.locator('canvas').boundingBox();
  const point = (x, y) => [rect.x + x * rect.width / 800, rect.y + y * rect.height / 600];
  await page.mouse.click(...point(160, 63));
  await expect(page.locator('#status')).toContainText('Server list unavailable');
  fail = false;
  await page.mouse.click(...point(665, 520));
  await expect(page.locator('#status')).toContainText('Choose a server, enter');
  await page.mouse.click(...point(180, 177));
  await page.keyboard.type('unapproved.invalid:3030', { delay: 25 });
  await page.mouse.click(...point(688, 563));
  await expect(page.locator('#status')).toContainText('Other addresses are not supported');
  expect(connections).toBe(0);
  await page.screenshot({ path: testInfo.outputPath('invalid-server.png') });
  // Selecting a row replaces the invalid text and restores a valid destination.
  await page.mouse.click(...point(240, 324));
  await page.mouse.click(...point(688, 563));
  await expect(page.locator('#status')).toHaveAttribute('data-state', 'playing', { timeout: 40000 });
  expect(connections).toBe(1);
  expect(errors).toEqual([]);
});
