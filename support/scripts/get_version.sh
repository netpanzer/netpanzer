#!/bin/sh

set -e

HERE="$(readlink -f "$(dirname "$0")")"

# If error, no string will be output
grep version: "$HERE/../../meson.build" 2>/dev/null | awk -F"'" '{print $2}' 2>/dev/null
