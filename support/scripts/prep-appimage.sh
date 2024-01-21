#!/bin/sh
# This script is not used by the CI, but meant to be run by anyone
# who wants to create any appimage.

if [ -z "$UID" ]; then
  echo "Could not detect UID."
  exit 1
fi

if [ ! -e "src/NetPanzer/Core/main.cpp" ];then
  echo "You need to be in the netpanzer source root directory when you run this script."
  exit 1
fi

export WORKSPACE="/np-src-root"
export VERSION=${VERSION:-"0.9.0-RC7"}

echo "Version is set to '$VERSION'"
echo "use 'VERSION=<version> $0' to change it."
echo "Waiting 10 seconds to start, hit CTRL-C now to cancel..."

read -t 10

set -ev

docker run -it --rm \
  -e VERSION=$VERSION  \
  -e ARCH=x86_64 \
  -e WORKSPACE \
  -e HOSTUID=$UID \
  -v $PWD:$WORKSPACE \
  andy5995/netpanzer-build-env:focal \
    /bin/bash -c 'usermod -u $HOSTUID npbuilder && \
    su npbuilder --command "$WORKSPACE/support/scripts/make-appimage.sh"'
