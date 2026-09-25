import net from 'node:net';
import { WebSocketServer, createWebSocketStream } from 'ws';

// Only deployment-controlled targets may be selected. Never accept a raw host
// or port from a browser: that would turn this into an arbitrary TCP proxy.
export function attachGateway(server, { host = '127.0.0.1', port = 3030,
  targets = [{ id: 'default', host, port }], origin, origins, maxClients = 32,
  maxClientsPerIP = maxClients, clientIP = request => request.socket.remoteAddress } = {}) {
  const allowed = new Map(targets.map(target => [target.id, { ...target }]));
  const wss = new WebSocketServer({ noServer: true, maxPayload: 1024 * 1024, perMessageDeflate: false });
  const connections = new Map();
  server.on('upgrade', (request, socket, head) => {
    const expectedOrigin = origin || `http://${request.headers.host}`;
    const address = clientIP(request);
    let url;
    try { url = new URL(request.url, 'http://localhost'); } catch { socket.destroy(); return; }
    const target = allowed.get(url.searchParams.get('server') || 'default');
    if (url.pathname !== '/game' || !target ||
        [...url.searchParams.keys()].some(key => key !== 'server') ||
        url.searchParams.getAll('server').length > 1 ||
        !(origins || [expectedOrigin]).includes(request.headers.origin) || wss.clients.size >= maxClients ||
        (connections.get(address) || 0) >= maxClientsPerIP) {
      socket.end('HTTP/1.1 403 Forbidden\r\nConnection: close\r\n\r\n');
      return;
    }
    wss.handleUpgrade(request, socket, head, ws => {
      connections.set(address, (connections.get(address) || 0) + 1);
      ws.on('close', () => {
        const count = connections.get(address) - 1;
        if (count) connections.set(address, count); else connections.delete(address);
      });
      wss.emit('connection', ws, target);
    });
  });
  wss.on('connection', (ws, { host, port }) => {
    const tcp = net.createConnection({ host, port });
    tcp.setNoDelay(true);
    const stream = createWebSocketStream(ws);
    const close = () => { tcp.destroy(); stream.destroy(); };
    tcp.on('error', () => { ws.close(1011, 'Game server unavailable'); close(); });
    stream.on('error', close);
    ws.on('close', close);
    tcp.on('end', () => ws.close(1000, 'Game connection closed'));
    tcp.on('close', () => ws.close(1000, 'Game connection closed'));
    stream.pipe(tcp).pipe(stream);
  });
  return wss;
}
