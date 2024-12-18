#!/bin/bash

if [ -z "$DOCKER_BUILD" ]; then
  echo "This script is intended to be run from a docker container."
  echo "See the docs at https://github.com/andy5995/linuxdeploy-build-helper-container"
  exit 1
fi

# Set bash options:
# -e: Exit immediately if a command exits with a non-zero status.
# -v: Print each command to stderr before executing it.
set -ev

# Set default workspace if not provided
WORKSPACE=${WORKSPACE:-$(pwd)}
# Check if the workspace path is absolute
if [[ "$WORKSPACE" != /* ]]; then
  echo "The workspace path must be absolute"
  exit 1
fi
test -d "$WORKSPACE"

# Set default source root if not provided
SOURCE_ROOT=${SOURCE_ROOT:-$WORKSPACE}
# Check if the source root path is absolute
if [[ "$SOURCE_ROOT" != /* ]]; then
  echo "The source root path must be absolute"
  exit 1
fi
# Verify that you're in the source root
echo $SOURCE_ROOT
test -f "$SOURCE_ROOT/src/NetPanzer/Core/main.cpp"

# Define and create application directory if it doesn't exist
# This is the directory where your project will be installed to
# and an AppImage created
APPDIR=${APPDIR:-"/tmp/$USER-AppDir"}
if [ -d "$APPDIR" ]; then
  rm -rf "$APPDIR"
else
  mkdir -v -p "$APPDIR"
fi

# Install necessary dependencies
# Required version of SDL2 and and SDL2-ttf are too old in Ubuntu Focal, which is
# the container distro for linuxdeploy.
# See https://github.com/netpanzer/netpanzer/issues/235
sudo apt update && sudo apt upgrade -y
sudo apt install -y \
  gettext \
  liblua5.1-0-dev \
  libphysfs-dev \
  libsdl2-mixer-dev

# Set up build directory
BUILD_DIR="$SOURCE_ROOT/appimage_build"
cd "$SOURCE_ROOT"
BUILD_DIR="$SOURCE_ROOT/_build_prep_appdir"
# Clean build directory if specified and it exists
if [ "$CLEAN_BUILD" = "true" ] && [ -d "$BUILD_DIR" ]; then
  rm -rf "$BUILD_DIR"
fi

# Setup project for building
if [ ! -d "$BUILD_DIR" ]; then
  meson setup "$BUILD_DIR" \
    -Dbuildtype=release \
    -Dstrip=true \
    -Db_sanitize=none \
    -Dprefix=/usr
fi

# Build project
cd "$BUILD_DIR"
ninja
meson install --destdir=$APPDIR --skip-subprojects

# Set up output directory
OUT_DIR="$WORKSPACE/out"
if [ ! -d "$OUT_DIR" ]; then
  mkdir "$OUT_DIR"
fi
cd "$OUT_DIR"

# Set LinuxDeploy output version
LINUXDEPLOY_OUTPUT_VERSION="$VERSION"
# Generate AppImage using linuxdeploy
linuxdeploy \
  --appdir=$APPDIR \
  -d $SOURCE_ROOT/support/win32/netpanzer.desktop \
  --custom-apprun=$SOURCE_ROOT/support/appimage/AppRun \
  --icon-file=$SOURCE_ROOT/netpanzer.png \
  --icon-filename=netpanzer \
  --executable $APPDIR/usr/bin/netpanzer \
  --output appimage
