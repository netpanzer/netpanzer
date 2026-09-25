import dgram from 'node:dgram';
import net from 'node:net';

// This file is operator input, never supplied by a browser. A small static
// allowlist also bounds status probes and prevents server-list SSRF.
export function validateServers(servers) {
  if (!Array.isArray(servers) || !servers.length || servers.length > 32)
    throw new Error('Server configuration must contain 1–32 entries.');
  const ids = new Set(), addresses = new Set();
  return servers.map(s => {
    if (!s || typeof s.id !== 'string' || !/^[a-zA-Z0-9_-]{1,64}$/.test(s.id) || ids.has(s.id) ||
        typeof s.host !== 'string' || !/^[a-zA-Z0-9.-]{1,253}$/.test(s.host) ||
        !Number.isInteger(s.port) || s.port < 1 || s.port > 65535 ||
        typeof s.name !== 'string' || !s.name.length || s.name.length > 100 ||
        (s.password !== undefined && typeof s.password !== 'boolean') ||
        addresses.has(`${s.host}:${s.port}`))
      throw new Error('Each server needs a unique id and host:port, a hostname/IPv4 address, port (1–65535) and name.');
    ids.add(s.id);
    addresses.add(`${s.host}:${s.port}`);
    return { id: s.id, host: s.host, port: s.port, name: s.name,
      ...(s.password === undefined ? {} : { password: s.password }) };
  });
}

function reachable({ host, port }) {
  return new Promise(resolve => {
    const socket = net.createConnection({ host, port });
    const finish = result => { clearTimeout(timer); socket.destroy(); resolve(result); };
    const timer = setTimeout(() => finish(false), 1000);
    socket.on('connect', () => finish(true));
    socket.on('error', () => finish(false));
  });
}

export function queryServer(target) {
  return new Promise(resolve => {
    const socket = dgram.createSocket('udp4');
    const started = Date.now();
    let finished = false;
    const finish = async fields => {
      if (finished) return;
      finished = true;
      clearTimeout(timer);
      socket.close();
      // Private servers often do not expose the optional UDP status service.
      // Show unknown metadata but still let players join a reachable server.
      resolve({ ...target, map: '?', players: -1, maxplayers: -1, ping: -1,
        protocol: -1, password: target.password || false, auth: false,
        running: fields ? true : await reachable(target), ...fields });
    };
    const timer = setTimeout(() => finish(null), 750);
    socket.on('error', () => finish(null));
    socket.on('message', data => {
      const tokens = data.toString('utf8').replace(/^\\/, '').split('\\');
      const fields = new Map();
      for (let i = 0; i + 1 < tokens.length; i += 2) fields.set(tokens[i], tokens[i + 1]);
      if (!fields.has('protocol') || !fields.has('hostname')) return;
      const number = key => /^\d{1,6}$/.test(fields.get(key)) ? Number(fields.get(key)) : -1;
      finish({ name: fields.get('hostname').slice(0, 100), map: (fields.get('mapname') || '?').slice(0, 80),
        players: number('numplayers'), maxplayers: number('maxplayers'), protocol: number('protocol'),
        password: fields.get('password') === 'y', auth: fields.get('authentication') === 'y', ping: Date.now() - started });
    });
    // Connected UDP accepts replies only from the configured endpoint.
    socket.connect(target.port, target.host, error => {
      if (error) finish(null);
      else if (!finished) socket.send('\\status\\final\\');
    });
  });
}

export function createServerCatalog(targets, query = queryServer) {
  let pending, cached, expires = 0;
  return () => {
    if (cached && Date.now() < expires) return Promise.resolve(cached);
    if (!pending) pending = Promise.all(targets.map(query)).then(servers => {
      cached = servers;
      expires = Date.now() + 5000;
      return cached;
    }).finally(() => { pending = null; });
    return pending;
  };
}
