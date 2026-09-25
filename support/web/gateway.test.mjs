import { test } from 'node:test';
import assert from 'node:assert/strict';
import http from 'node:http';
import net from 'node:net';
import { once } from 'node:events';
import { WebSocket } from 'ws';
import { attachGateway } from './gateway.mjs';

async function fixture(t, { echo = true } = {}) {
  const tcp = net.createServer(socket => { if (echo) socket.pipe(socket); else socket.end(); });
  tcp.listen(0, '127.0.0.1');
  await once(tcp, 'listening');
  const server = http.createServer();
  const gateway = attachGateway(server, { port: tcp.address().port, origin: 'http://localhost' });
  server.listen(0, '127.0.0.1');
  await once(server, 'listening');
  t.after(() => { for (const ws of gateway.clients) ws.terminate(); gateway.close(); server.close(); tcp.close(); });
  return { url: `ws://127.0.0.1:${server.address().port}`, tcp };
}

test('gateway preserves binary bytes across fragmented WebSocket messages and TCP chunks', { timeout: 5000 }, async t => {
  const { url } = await fixture(t);
  const ws = new WebSocket(`${url}/game`, 'binary', { origin: 'http://localhost' });
  t.after(() => ws.terminate());
  await once(ws, 'open');
  const payload = Buffer.alloc(180000);
  for (let i = 0; i < payload.length; i++) payload[i] = i % 256;
  const received = [];
  let length = 0;
  const done = new Promise(resolve => ws.on('message', (chunk, binary) => {
    assert.equal(binary, true);
    received.push(chunk);
    length += chunk.length;
    if (length >= payload.length) resolve();
  }));
  ws.send(payload.subarray(0, 713), { fin: false });
  ws.send(payload.subarray(713), { fin: true });
  await done;
  assert.deepEqual(Buffer.concat(received), payload);
});

test('gateway rejects other origins and paths', { timeout: 5000 }, async t => {
  const { url } = await fixture(t);
  for (const [suffix, origin] of [['/game', 'http://untrusted.invalid'], ['/other', 'http://localhost'],
    ['/game?server=unapproved', 'http://localhost'], ['/game?host=127.0.0.1&port=22', 'http://localhost'],
    ['/game?server=default&server=other', 'http://localhost']]) {
    const ws = new WebSocket(url + suffix, { origin });
    await assert.rejects(once(ws, 'open'), /403/);
  }
});

test('server selection routes to the selected allowlisted backend', { timeout: 5000 }, async t => {
  const backends = await Promise.all(['first', 'second'].map(async label => {
    const tcp = net.createServer(socket => socket.end(label));
    tcp.listen(0, '127.0.0.1');
    await once(tcp, 'listening');
    t.after(() => tcp.close());
    return { id: label, host: '127.0.0.1', port: tcp.address().port };
  }));
  const server = http.createServer();
  const gateway = attachGateway(server, { targets: backends, origin: 'http://localhost' });
  server.listen(0, '127.0.0.1');
  await once(server, 'listening');
  t.after(() => { for (const ws of gateway.clients) ws.terminate(); gateway.close(); server.close(); });
  for (const { id } of backends) {
    const ws = new WebSocket(`ws://127.0.0.1:${server.address().port}/game?server=${id}`, { origin: 'http://localhost' });
    const [bytes] = await once(ws, 'message');
    assert.equal(bytes.toString(), id);
    ws.close();
  }
});

test('closing the TCP server closes the browser connection', { timeout: 5000 }, async t => {
  const { url } = await fixture(t, { echo: false });
  const ws = new WebSocket(`${url}/game`, { origin: 'http://localhost' });
  const [code] = await once(ws, 'close');
  assert.equal(code, 1000);
});
