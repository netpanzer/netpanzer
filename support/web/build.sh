#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/../.."
command -v emcc >/dev/null || { echo 'Activate Emscripten first: source /path/to/emsdk/emsdk_env.sh' >&2; exit 1; }
build_dir="${1:-build-web}"
if [[ -f "$build_dir/build.ninja" ]]; then
  meson setup --reconfigure "$build_dir"
else
  meson setup "$build_dir" --cross-file support/cross/emscripten.ini \
    --force-fallback-for=lua-5.1,physfs -Dlua:interpreter=false -Dlua:loadlib=false
fi
meson compile -C "$build_dir" -j "${JOBS:-4}" netpanzer:executable
