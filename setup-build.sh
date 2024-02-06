#!/bin/sh

BUILDDIR=$1
set -ev
meson setup "${@}"

# I think I set these properly in the meson build now
# -Dfreetype2:png=disabled -Dfreetype2:zlib=disabled

set +v
echo Now cd to $BUILDDIR and run ninja

exit 0
