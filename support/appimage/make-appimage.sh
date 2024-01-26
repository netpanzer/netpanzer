#!/bin/sh
# To create an AppImage from the CI, only this script is neeed and
# prep-appimage.sh is not used.

set -ev

if [ -z "$WORKSPACE" ]; then
  echo "Workspace needs to be set. If you're buildling locally, use prepimage.sh."
  echo "If calling from a GitHub workflow, be sure to check and set the WORKSPACE"
  echo "environmental variable."
  exit 1
fi

if [ -z "$VERSION" ]; then
  echo "VERSION is set from prep-appimage.sh or the GitHub workflow."
  exit 1
fi

export LINUXDEPLOY_OUTPUT_VERSION=$VERSION

OUTPUT_DIR="$WORKSPACE/support"
test -d "$OUTPUT_DIR"
APPDIR="$OUTPUT_DIR/AppDir"
BUILD_DIR="$WORKSPACE/support/docker_build"

cd "$WORKSPACE"

if [ "$CLEAN_BUILD" = "true" ]; then
  rm -rf "$BUILD_DIR"
fi

SETUP_CMD=""
if [ ! -d $BUILD_DIR ]; then
  SETUP_CMD="./setup-build.sh $BUILD_DIR"
else
  cd $BUILD_DIR
  SETUP_CMD="meson configure"
fi

$SETUP_CMD \
  -Dbuildtype=release \
  -Dstrip=true \
  -Db_sanitize=none \
  -Dprefix=/usr \
  -Ddatadir=/usr/data \
  -Dlocaledir=/usr/locale

cd $BUILD_DIR

if [ -d "$APPDIR" ]; then
  rm -rf "$APPDIR"
fi
ninja
meson install --destdir $APPDIR --skip-subprojects

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
  --custom-apprun=$WORKSPACE/support/appimage/AppRun \
  --icon-file="$WORKSPACE/netpanzer.png" \
  --icon-filename=netpanzer \
  --executable "$APPDIR/usr/bin/netpanzer" \
  --appdir "$APPDIR" \
  --output appimage
sha256sum NetPanzer-${VERSION}-${ARCH}.AppImage > NetPanzer-${VERSION}-${ARCH}.AppImage.sha256sum
