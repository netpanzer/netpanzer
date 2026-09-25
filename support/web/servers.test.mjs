import { test } from 'node:test';
import assert from 'node:assert/strict';
import { once } from 'node:events';
import dgram from 'node:dgram';
import net from 'node:net';
import { validateServers, queryServer, createServerCatalog } from './servers.mjs';

const entry = { id: 'local', name: 'Local', host: '127.0.0.1', port: 3030 };

test('server configuration validates destinations and rejects duplicates', () => {
  assert.deepEqual(validateServers([entry]), [entry]);
  for (const entries of [[], [entry, entry], [{ ...entry, port: 0 }],
    [{ ...entry, host: 'http://localhost' }], [{ ...entry, id: '../game' }],
    [{ ...entry, id: 'second' }, entry], [{ ...entry, id: undefined }],
    [{ ...entry, id: 123 }], [{ ...entry, password: 'yes' }]]) assert.throws(() => validateServers(entries));
});

test('server list reads live UDP metadata including password and protocol', async t => {
  const udp = dgram.createSocket('udp4');
  udp.bind(0, '127.0.0.1');
  await once(udp, 'listening');
  t.after(() => udp.close());
  udp.on('message', (data, peer) => {
    assert.equal(data.toString(), '\\status\\final\\');
    udp.send('gamename\\netpanzer\\hostname\\Test match\\mapname\\Two clans\\numplayers\\3\\maxplayers\\8\\protocol\\1128\\password\\y\\authentication\\n\\final\\', peer.port, peer.address);
  });
  const result = await queryServer({ ...entry, port: udp.address().port });
  assert.equal(result.name, 'Test match');
  assert.equal(result.map, 'Two clans');
  assert.equal(result.players, 3);
  assert.equal(result.maxplayers, 8);
  assert.equal(result.protocol, 1128);
  assert.equal(result.password, true);
  assert.equal(result.auth, false);
  assert.equal(result.running, true);
  assert.ok(result.ping >= 0);
});

test('private TCP servers stay joinable without UDP status; offline servers time out', async t => {
  const tcp = net.createServer(socket => socket.end());
  tcp.listen(0, '127.0.0.1');
  await once(tcp, 'listening');
  const target = { ...entry, port: tcp.address().port };
  t.after(() => tcp.close());
  const result = await queryServer(target);
  assert.equal(result.running, true);
  assert.equal(result.players, -1);
  assert.equal(result.map, '?');
  await new Promise(resolve => tcp.close(resolve));
  assert.equal((await queryServer(target)).running, false);
});

test('concurrent and repeated refresh requests share cached status probes', async () => {
  let queries = 0;
  const catalog = createServerCatalog([entry], async s => { queries++; return s; });
  await Promise.all([catalog(), catalog(), catalog()]);
  assert.deepEqual(await catalog(), [entry]);
  assert.equal(queries, 1);
});

test('unresolvable hostnames are offline rather than crashing the gateway', { timeout: 5000 }, async () => {
  const result = await queryServer({ ...entry, host: 'nonexistent.invalid' });
  assert.equal(result.running, false);
});
