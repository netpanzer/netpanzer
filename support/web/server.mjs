import http from 'node:http';
import net from 'node:net';
import { createReadStream } from 'node:fs';
import { access, mkdir, mkdtemp, readFile, stat, writeFile } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import path from 'node:path';
import { fileURLToPath } from 'node:url';
import { spawn } from 'node:child_process';
import { parseArgs } from 'node:util';
import { attachGateway } from './gateway.mjs';
import { validateServers, createServerCatalog } from './servers.mjs';

const root = fileURLToPath(new URL('../../', import.meta.url));
const { values } = parseArgs({ options: {
  port: { type: 'string', default: '8080' },
  bind: { type: 'string', default: '127.0.0.1' },
  dir: { type: 'string', default: path.join(root, 'build-web') },
  native: { type: 'string' },
  bots: { type: 'string', default: '2' },
  'game-port': { type: 'string', default: '3030' },
  'game-host': { type: 'string', default: '127.0.0.1' },
  origin: { type: 'string' },
  servers: { type: 'string' },
} });
const webPort = Number(values.port), gamePort = Number(values['game-port']), bots = Number(values.bots);
if (![webPort, gamePort].every(p => Number.isInteger(p) && p > 0 && p < 65536) || !Number.isInteger(bots) || bots < 0 || bots > 6) {
  throw new Error('Ports must be 1–65535; bots must be 0–6.');
}
await access(path.join(values.dir, 'netpanzer.wasm'));
const targets = validateServers(values.servers ? JSON.parse(await readFile(values.servers, 'utf8')) :
  [{ id: 'default', name: 'NetPanzer browser battlefield', host: values['game-host'], port: gamePort }]);
if (values.native && !targets.some(s => s.host === '127.0.0.1' && s.port === gamePort))
  throw new Error('--servers must include the local --native server.');
const getServers = createServerCatalog(targets);
const children = [];
let shuttingDown = false;
function stop() {
  if (shuttingDown) return;
  shuttingDown = true;
  for (const child of children) child.kill('SIGTERM');
  for (const ws of gateway.clients) ws.terminate();
  server.close();
}
const contentTypes = { '.html': 'text/html; charset=utf-8', '.js': 'text/javascript; charset=utf-8', '.wasm': 'application/wasm', '.data': 'application/octet-stream' };
const files = new Set(['index.html', 'launcher.js', 'netpanzer.js', 'netpanzer.wasm', 'netpanzer.data']);
const server = http.createServer(async (req, res) => {
  const name = new URL(req.url, 'http://localhost').pathname.slice(1) || 'index.html';
  if (name === 'servers' && req.method === 'GET') {
    try {
      const servers = await getServers();
      res.writeHead(200, { 'Content-Type': 'application/json', 'Cache-Control': 'no-store', 'X-Content-Type-Options': 'nosniff' });
      res.end(JSON.stringify(servers));
    } catch { res.writeHead(503).end(); }
    return;
  }
  if (!files.has(name) || !['GET', 'HEAD'].includes(req.method)) { res.writeHead(404).end(); return; }
  try {
    const filename = path.join(values.dir, name);
    const info = await stat(filename);
    res.writeHead(200, { 'Content-Type': contentTypes[path.extname(name)], 'Content-Length': info.size, 'Cache-Control': 'no-cache', 'X-Content-Type-Options': 'nosniff' });
    if (req.method === 'HEAD') res.end();
    else createReadStream(filename).on('error', () => res.destroy()).pipe(res);
  } catch { res.writeHead(404).end(); }
});
const gateway = attachGateway(server, { targets, origin: values.origin });
process.on('SIGINT', stop);
process.on('SIGTERM', stop);
process.on('exit', () => { for (const child of children) child.kill('SIGTERM'); });

async function waitForGame() {
  const deadline = Date.now() + 30000;
  while (Date.now() < deadline) {
    if (shuttingDown) throw new Error('Game process stopped.');
    const ready = await new Promise(resolve => {
      const socket = net.createConnection({ host: '127.0.0.1', port: gamePort });
      socket.once('connect', () => { socket.destroy(); resolve(true); });
      socket.once('error', () => resolve(false));
    });
    if (ready) return;
    await new Promise(resolve => setTimeout(resolve, 200));
  }
  throw new Error('Native game server did not start within 30 seconds.');
}

if (values.native) {
  if (values['game-host'] !== '127.0.0.1') throw new Error('--native requires --game-host 127.0.0.1');
  const binary = path.resolve(values.native);
  await access(binary);
  // Refuse to confuse a pre-existing server with the one this process owns.
  const probe = net.createServer();
  await new Promise((resolve, reject) => { probe.once('error', reject); probe.listen(gamePort, '127.0.0.1', resolve); });
  await new Promise(resolve => probe.close(resolve));
  const profiles = await mkdtemp(path.join(tmpdir(), 'netpanzer-web-'));
  console.log(`Isolated game profiles and logs: ${profiles}`);
  async function launch(name, args, config) {
    const profile = path.join(profiles, name);
    const configDir = path.join(profile, '.netpanzer', 'config');
    await mkdir(configDir, { recursive: true });
    await writeFile(path.join(configDir, `${name}.cfg`), config);
    const child = spawn(binary, [...args, '--master_server=none', `--game_config=/config/${name}.cfg`], {
      cwd: root, stdio: ['ignore', 'inherit', 'inherit'],
      env: { ...process.env, NETPANZER_DATADIR: path.join(root, 'data'), NETPANZER_USERDIR: profile + path.sep },
    });
    children.push(child);
    child.on('error', error => { console.error(`${name}: ${error.message}`); process.exitCode = 1; stop(); });
    child.on('exit', (code, signal) => {
      if (!shuttingDown) { console.error(`${name} exited (${code ?? signal})`); process.exitCode = 1; stop(); }
    });
  }
  await launch('server', ['--dedicated', `--port=${gamePort}`], `server.bindaddress = "127.0.0.1"
server.public = false
server.interactive_console = false
server.name = "NetPanzer browser battlefield"
game.mapcycle = "Two clans"
game.maxplayers = 8
game.maxunits = 160
`);
  await waitForGame();
  for (let i = 0; i < bots; i++) {
    await launch(`bot${i}`, [`--bot=127.0.0.1:${gamePort}`], `player.name = "Bot ${i + 1}"\nbot.allied = false\n`);
  }
}
server.on('error', error => { console.error(error.message); process.exitCode = 1; stop(); });
server.listen(webPort, values.bind, () => console.log(`Play at http://${values.bind}:${webPort} (game → ${values['game-host']}:${gamePort})`));
