/* The game owns the canvas; this page only handles loading and browser UI. */
const canvas = document.getElementById('canvas');
const statusLabel = document.getElementById('status');
const playButton = document.getElementById('play');
// Versioned assets can live separately from the stable, same-origin gateway.
const launchOptions = new URLSearchParams(location.search);
const networkBase = new URL(launchOptions.get('network') || './', location.href);
if (networkBase.origin !== location.origin) throw new Error('The game gateway must be same-origin');
if (launchOptions.get('embedded') === '1') document.documentElement.classList.add('embedded');
const gateway = new URL('game', networkBase);
gateway.protocol = location.protocol === 'https:' ? 'wss:' : 'ws:';
let started = false;
let offeredServers = [];
let serverRequest = 0;
var Module = {
  canvas,
  websocket: { url: gateway.href, subprotocol: 'binary' },
  print: (...args) => console.log(...args),
  printErr: (...args) => console.error(...args),
  setStatus: text => { if (!started && text) statusLabel.textContent = text; },
  onRuntimeInitialized() {
    statusLabel.textContent = 'Ready. Open the game, then choose Join and a server.';
    playButton.textContent = 'Play NetPanzer';
    playButton.disabled = false;
  },
  onAbort(reason) {
    statusLabel.textContent = `Unable to run the game: ${reason}. Reload to retry.`;
    playButton.disabled = true;
  },
  onGameStatus(text, state) {
    statusLabel.textContent = text;
    statusLabel.dataset.state = state;
  },
  async refreshGameServers(request) {
    serverRequest = request;
    try {
      const response = await fetch(new URL('servers', networkBase), { signal: AbortSignal.timeout(10000) });
      if (!response.ok) throw new Error('Server list unavailable');
      const servers = await response.json();
      if (serverRequest !== request) return;
      offeredServers = servers;
      for (const s of servers) {
        Module.ccall('netpanzer_web_server', null,
          ['number', 'string', 'number', 'string', 'string', ...Array(7).fill('number')],
          [request, s.host, s.port, s.name, s.map, s.players, s.maxplayers, s.ping,
            s.protocol, s.password, s.auth, s.running]);
      }
      Module.ccall('netpanzer_web_servers_done', null, ['number', 'string'],
        [request, 'No compatible servers found. Try Refresh or enter a server address.']);
      Module.onGameStatus('Choose a server, enter your player name, then click Play.', 'menu');
    } catch (error) {
      if (serverRequest !== request) return;
      Module.ccall('netpanzer_web_servers_done', null, ['number', 'string'],
        [request, 'Server list unavailable. Click Refresh to retry.']);
      Module.onGameStatus('Server list unavailable. Click Refresh to retry.', 'menu');
    }
  },
  cancelGameServerRefresh() { serverRequest = 0; },
  selectGameServer(address) {
    const server = offeredServers.find(s => address === `${s.host}:${s.port}` ||
      (s.port === 3030 && address === s.host));
    if (!server) {
      if (launchOptions.get('public') === '1' && /^[a-zA-Z0-9.-]+(?::[0-9]{1,5})?$/.test(address)) {
        const destination = address.includes(':') ? address : `${address}:3030`;
        gateway.search = new URLSearchParams({ server: destination }).toString();
        Module.websocket.url = gateway.href;
        Module.onGameStatus('Enter the server password, or leave it blank, then click OK.', 'menu');
        return 2;
      }
      Module.onGameStatus('Select a server from this website\'s list. Other addresses are not supported.', 'menu');
      return false;
    }
    gateway.search = new URLSearchParams({ server: server.id }).toString();
    Module.websocket.url = gateway.href;
    return server.password ? 2 : 1;
  },
  onGameExit() {
    serverRequest = 0;
    if (!['error', 'disconnected'].includes(statusLabel.dataset.state))
      statusLabel.textContent = 'Session ended.';
    playButton.textContent = 'Play again';
    playButton.disabled = false;
    playButton.onclick = () => location.reload();
  },
};
// Keep the canvas's CSS box aligned with its pixels, including letterboxed
// fullscreen and in-game resolution changes, so SDL maps the mouse correctly.
const updateAspect = () => canvas.style.setProperty('--game-ratio', canvas.width / canvas.height);
new MutationObserver(updateAspect).observe(canvas, { attributes: true, attributeFilter: ['width', 'height'] });
updateAspect();
canvas.addEventListener('contextmenu', event => event.preventDefault());
canvas.addEventListener('pointerdown', () => canvas.focus({ preventScroll: true }));
canvas.addEventListener('keydown', event => {
  if (['ArrowUp', 'ArrowDown', 'ArrowLeft', 'ArrowRight', ' ', 'Tab', 'F1'].includes(event.key)) event.preventDefault();
});
playButton.onclick = () => {
  if (started) return;
  started = true;
  playButton.disabled = true;
  playButton.textContent = 'Playing';
  Module.onGameStatus('Choose Join to select a server. Hosting is unavailable in the browser.', 'menu');
  canvas.focus({ preventScroll: true });
  try {
    Module.callMain(['--master_server=none']);
  } catch (error) {
    Module.onAbort(error.message || error);
  }
};
document.getElementById('fullscreen').onclick = async () => {
  try {
    if (document.fullscreenElement) await document.exitFullscreen();
    else await canvas.parentElement.requestFullscreen();
    canvas.focus({ preventScroll: true });
  } catch (error) { statusLabel.textContent = `Fullscreen unavailable: ${error.message}`; }
};
window.addEventListener('error', event => Module.onAbort(event.message || 'Download failed'));
