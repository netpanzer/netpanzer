#! /bin/bash

if [ ! -f RELEASE_VERSION ]; then
    echo "ERROR: RELEASE_VERSION file missing, cannot create"
    exit 1
fi

RELEASEDIR="releases"
VERSION="`<RELEASE_VERSION`"
NPDEST=/tmp/netpanzer
ZIPNAME="netpanzer-${VERSION}.zip"
LINEXENAME="build/crosslinux/release/netpanzer"
WINEXENAME="build/crossmingw/release/netpanzer.exe"

PATH=$PATH:/usr/local/gcc/i686-linux/bin/

echo "Compiling Linux version ${VERSION}"

mkdir -p build/crosslinux/{release,debug}/libs
ln -s "`i686-linux-g++ -print-file-name=libstdc++.a`" build/crosslinux/release/libs/libstdc++.a
ln -s "`i686-linux-g++ -print-file-name=libstdc++.a`" build/crosslinux/debug/libs/libstdc++.a

scons crosslinux \
      crosslinuxcompilerprefix=i686-linux- \
      crosslinuxsdlconfig=/usr/local/gcc/i686-linux/bin/sdl-config

i686-linux-strip "${LINEXENAME}"

[ -d "${NPDEST}" ] && rm -rf "${NPDEST}"

mkdir -p "${NPDEST}"
cp "${LINEXENAME}" "${NPDEST}"

echo "Compiling Windows version ${VERSION}"

scons crossmingw \
      crossmingwcompilerprefix=i686-mingw32-\
      crossmingwsdlconfig=/usr/local/gcc/mingw-3.4.5/bin/sdl-config

i686-mingw32-strip "${WINEXENAME}"

cp "${WINEXENAME}" "${NPDEST}"

support/scripts/copy_datafiles.sh "${NPDEST}"
support/scripts/copy_docfiles.sh "${NPDEST}"

cp /usr/local/gcc/mingw-3.4.5/usr/bin/SDL.dll "${NPDEST}"
cp /usr/local/gcc/mingw-3.4.5/usr/lib/*.dll "${NPDEST}"

[ ! -d "${RELEASEDIR}" ] && mkdir "${RELEASEDIR}"

find "${NPDEST}" -name "*/.*" -exec rm -f "{}" \;

pushd "${NPDEST}/.."
zip -9r "${ZIPNAME}" `basename "${NPDEST}"`
popd

[ -f "${RELEASEDIR}/${ZIPNAME}" ] && rm -f "${RELEASEDIR}/${ZIPNAME}"

mv "${NPDEST}/../${ZIPNAME}" "${RELEASEDIR}"
rm -rf "${NPDEST}"

support/macosx/make_appbundle.sh

