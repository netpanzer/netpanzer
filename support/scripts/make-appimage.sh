#!/bin/sh

set -ev

if [ -z "$WORKSPACE" ]; then
  echo "This script needs to be called from prep-appimage.sh"
  exit 1
fi

if [ -z "$VERSION" ]; then
  echo "VERSION is set from prep-appimage.sh or the GitHub workflow."
  exit 1
fi

OUTPUT_DIR="$WORKSPACE/support"
test -d "$OUTPUT_DIR"
APPDIR="$OUTPUT_DIR/AppDir"
BUILD_DIR="$WORKSPACE/support/docker_build"

cd "$WORKSPACE"
# meson setup "$BUILD_DIR" -Dbuildtype=release -Dstrip=true -Db_sanitize=none -Dprefix=/usr

rm -rf "$APPDIR"
scons -j 4
mkdir -p "$APPDIR/usr/bin"
mkdir "$APPDIR/usr/share"
cp netpanzer "$APPDIR/usr/bin"

datadirs="cache \
  maps \
  pics \
  powerups \
  scripts \
  sound \
  units \
  wads"
for dir in $datadirs; do
  cp -a $dir "$APPDIR/usr/data"
done

cd "$OUTPUT_DIR"
#if [ ! -e "linuxdeploy-${ARCH}.AppImage" ]; then
  #curl -LO https://github.com/linuxdeploy/linuxdeploy/releases/download/1-alpha-20231206-1/linuxdeploy-${ARCH}.AppImage
  #chmod +x linuxdeploy-${ARCH}.AppImage
  #./linuxdeploy-${ARCH}.AppImage --appimage-extract
#fi

# This should set the name of the output appimage but it doesn't
export NAME="netpanzer"
"$TOOLS_DIR/squashfs-root/AppRun"  \
  -d "$WORKSPACE/support/win32/netpanzer.desktop" \
  --custom-apprun=$WORKSPACE/support/scripts/AppRun \
  --icon-file="$WORKSPACE/netpanzer.png" \
  --icon-filename=netpanzer \
  --executable "$APPDIR/usr/bin/netpanzer" \
  --appdir "$APPDIR" \
  --output appimage
sha256sum NetPanzer-${VERSION}-${ARCH}.AppImage > NetPanzer-${VERSION}-${ARCH}.AppImage.sha256sum
