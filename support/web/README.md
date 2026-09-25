# NetPanzer in the browser

The original C++ client compiles to WebAssembly using Emscripten's SDL2,
SDL2_ttf and SDL2_mixer ports. Graphics, input, sound, Lua scripts and game
assets run locally in the browser. A small WebSocket gateway forwards the
existing game protocol to a native NetPanzer dedicated server.

```text
Chrome / Firefox ── HTTP ──> static game files
                 └─ WebSocket /game ──> gateway ── TCP ──> NetPanzer server
                                                          ↑ native bots
```

## Build

Install Meson, Ninja, Python 3, Node.js 20+ and the desktop build prerequisites
in [BUILD.md](../../BUILD.md). Install and activate Emscripten 4.0.15:

```sh
git clone https://github.com/emscripten-core/emsdk.git /path/to/emsdk
/path/to/emsdk/emsdk install 4.0.15
/path/to/emsdk/emsdk activate 4.0.15
source /path/to/emsdk/emsdk_env.sh

# From the NetPanzer repository root:
bash support/web/build.sh
```

The first build downloads and compiles SDL ports; subsequent builds reuse
the SDK cache. Lua and PhysicsFS use the repository's pinned Meson wraps.
`JOBS=8 bash support/web/build.sh` changes the default four parallel jobs.
Pass an optional output directory as the script's first argument.

Build a native server separately (this also checks the desktop code path):

```sh
meson setup build-web-native -Dbuildtype=release -Db_sanitize=none -Dnls=false -Dbuild_tests=false
meson compile -C build-web-native -j 4 netpanzer:executable
npm ci --prefix support/web
```

## Play locally

```sh
node support/web/server.mjs --native build-web-native/netpanzer
```

Open <http://127.0.0.1:8080> in Chrome or Firefox. Wait for the asset download,
then click **Play NetPanzer** to open the original main menu. Click **Join**,
enter your player name, select a server from the list, and click the in-game
**Play** button. Pick a unit style, then a flag to spawn. Drag
with the left mouse button to select tanks; left-click the ground to move or
an enemy to attack. Right-drag or use arrow keys to scroll the map. F1 opens
the game's help.

The runner starts one private dedicated server and two native bots. Open a
second browser to join the same match. Ctrl+C stops the runner and its game
processes. Profiles and logs are isolated in a printed temporary directory;
your desktop configuration is untouched. The `NETPANZER_USERDIR` environment
variable selects a profile's parent directory.

Options:

- `--bots 0` disables bots (0–6 allowed).
- `--port 8081` changes the HTTP/WebSocket port.
- `--game-port 3031` changes the native server's TCP port.
- `--dir /path/to/build-web` serves a different build.
- `--bind 0.0.0.0` allows access from another machine for LAN testing.
- Omit `--native` to use an existing server; select its fixed address with
  `--game-host 127.0.0.1 --game-port 3030`.
- `--servers /path/to/servers.json` offers multiple approved servers in the
  game's usual Join screen. See [servers.example.json](servers.example.json).

For example, after starting native servers on ports 3030 and 3031:

```sh
node support/web/server.mjs --servers support/web/servers.example.json
```

Each entry needs a unique `id`, `name`, `host` (hostname or IPv4) and `port`.
The file is read at startup. It can include local or remote native servers;
the gateway machine must be able to reach them. With `--native`, the file
must include that local server; the runner still launches only one server.

The browser fetches `/servers` and uses `/game?server=ID` to join the selected
entry. The gateway rejects unknown IDs and arbitrary destination parameters.
Manually typed addresses must also match a configured entry. This is an
operator-managed list, not the desktop public master-server directory.

The gateway queries the usual UDP status service for map, players, protocol,
password and authentication flags. Ping measures **gateway → game server**,
not the browser's complete latency. Probes are shared and cached for five
seconds. Private servers without UDP status get a TCP reachability check;
their details appear as `?` and compatibility is checked during connection.
If such a private server requires a password, set `"password": true` in its
entry to enable the original password prompt. Unreachable servers show a
timeout. Refresh retries the list without reloading the game.

The browser hides **Host**, desktop resolution selection and SDL fullscreen.
Use the page's Fullscreen button and resize the browser instead. Shadows,
smoke, minimap size, scrolling and audio controls remain available. **Resign**
returns to the menu for another server selection; **Exit netPanzer** ends the
client, after which **Play again** reloads it. These restrictions are web-only.
Desktop Alt+Enter and F9 shortcuts are disabled; use browser fullscreen and
your browser/OS screenshot tools rather than saving to a temporary virtual file.

## Tests

With the local runner listening on port 8080:

```sh
npm test --prefix support/web
cd support/web
PLAYWRIGHT_SKIP_BROWSER_GC=1 npx playwright install firefox
npm run test:browser
```

The browser tests use installed Google Chrome and Playwright's Firefox. They
check menu startup, options, server selection, connection and map synchronization,
fullscreen mouse coordinates, quick menu clicks, spawning, selection, actual
outgoing movement commands, camera input, resign/rejoin, failed-list retries and
rejection of unapproved addresses. Screenshots go into
`support/web/test-results/`; browser logs
are attached to the Playwright test result.

The gateway tests cover binary transport, destination routing, forbidden targets,
UDP metadata, private/offline servers and shared query caching. Windows and macOS desktop
builds have not been tested as part of this port. To run the native suite:

```sh
meson configure build-web-native -Dbuild_tests=true
meson devenv -C build-web-native meson test --suite netpanzer --print-errorlogs
```

## Website deployment

The `Browser client` GitHub Actions workflow builds and tests Chrome/Firefox.
Push `web-<full commit SHA>` to publish a checksummed browser prerelease without
changing the desktop latest release. `package-release.py` includes gzip assets,
file checksums, the source revision and GPL license. The Django `np-website`
repository pins this artifact in `web-release.json`, installs it outside its
deployment directory and provides an ASGI public-server gateway at `/play/game`.
See that repository's `deploy/BROWSER.md` for promotion and rollback. The local
Node runner described above remains useful for isolated tests with private bots;
its fixed allowlist is not the production public-server policy.

Publish `index.html`, `launcher.js`, `netpanzer.js`, `netpanzer.wasm` and
`netpanzer.data` together. Serve `.wasm` as `application/wasm`. Enable HTTP
compression and caching for the assets: the uncompressed data pack is about
125 MB and contains all maps, unit styles, sounds and fonts.

Use HTTPS and proxy `/servers` HTTP requests and `/game` WebSocket upgrades
(including the query string) to the gateway. Pass
`--origin https://your-site.example` to the gateway when TLS terminates at
the reverse proxy. The launcher automatically selects `wss:` on HTTPS.
The gateway has an explicit backend allowlist, an origin check, a 32-connection limit,
a 1 MiB message limit and stream backpressure. The supplied HTTP server is
for development; use your normal production web server for static assets,
TLS, access controls and rate limits.

If served under a subdirectory, the launcher requests `game` relative to the
page; rewrite the game and servers paths to `/game` and `/servers` when
proxying to this runner, preserving query strings. The build is
single-threaded and does not require COOP/COEP or SharedArrayBuffer.

Keep a native dedicated server running on the backend. Static hosting alone
can load the client but cannot provide multiplayer. Browser players can share
a server with native clients. Browsers do not expose raw TCP/UDP or listening
sockets, so the browser Join action uses the configured gateway. Browser hosting
would require a relay architecture or backend server provisioning, not simply
re-enabling Host. The desktop public master-server directory is not automatically
imported; add approved destinations to the website's server configuration.
See [Emscripten networking](https://emscripten.org/docs/porting/networking.html)
and [SDL's Emscripten notes](https://wiki.libsdl.org/SDL2/README-emscripten).

This first port targets desktop keyboards and mice. Browser settings live in
the tab's virtual filesystem and reset on reload. Mobile controls, persistent
settings, automatic public-directory discovery and splitting the initial asset download
are follow-up work. Preserve the game's and dependencies' license notices and
provide the corresponding source when distributing the compiled game.
