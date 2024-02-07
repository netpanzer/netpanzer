#!/bin/sh

BUILDDIR=$1
set -ev
meson setup "${@}"

set +v
echo Now cd to $BUILDDIR and run ninja

exit 0
