#!/bin/sh
# This script is not used by the CI, but meant to be run by anyone
# who wants to create any appimage.

if [ -z "$UID" ]; then
  UID=$(id -u)
fi

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

if [ -z "$GITHUB_WORKSPACE" ]; then
  echo "Version is set to '$VERSION'"
  echo "use 'VERSION=<version> $0' to change it."
  echo "Waiting 10 seconds to start, hit CTRL-C now to cancel..."
  read -t 10
fi

export -p

export CLEAN_BUILD=false
if [ "${1}" = "clean-build" ]; then
  CLEAN_BUILD=true
fi

# ARCH is used in the LinuxDeploy filename
export ARCH=${ARCH:-"x86_64"}
PLATFORM=${PLATFORM:-"amd64"}

if [ "$ARCH" = "x86_64" ]; then
  PLATFORM="amd64"
else
  PLATFORM="arm64"
fi

set -ev

docker run -t --rm \
  --platform "linux/$PLATFORM" \
  -e VERSION  \
  -e ARCH \
  -e WORKSPACE \
  -e HOSTUID=$UID \
  -e CLEAN_BUILD \
  -v $PWD:$WORKSPACE \
  andy5995/netpanzer-build-env:focal \
    /bin/bash -c 'usermod -u $HOSTUID npbuilder && \
      su npbuilder --command "$WORKSPACE/support/appimage/make-appimage.sh"'
