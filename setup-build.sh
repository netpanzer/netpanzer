#!/bin/sh

BUILDDIR=$1
set -ev
meson setup "${@}" -Dfreetype2:png=disabled -Dfreetype2:zlib=disabled
cd "$BUILDDIR"

set +v
echo Now cd to $BUILDDIR and run ninja

exit 0
