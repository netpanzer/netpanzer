#! /bin/bash

if [ ! -f RELEASE_VERSION ]; then
    echo "ERROR: RELEASE_VERSION file missing, cannot create"
    exit 1
fi

RELEASEDIR="releases"
VERSION="`<RELEASE_VERSION`"
NPDEST=/tmp/netpanzer
ZIPNAME="netpanzer-windows-${VERSION}.zip"
EXENAME="build/crossmingw/release/netpanzer.exe"

echo "Making netPanzer windows version ${VERSION}"

scons crossmingw \
      crossmingwcompilerprefix=i586-mingw32msvc-\
      crossmingwsdlconfig=/usr/local/bin/sdl-config-mingw

i586-mingw32msvc-strip "${EXENAME}"

[ -d "${NPDEST}" ] && rm -rf "${NPDEST}"

mkdir -p "${NPDEST}"
cp "${EXENAME}" "${NPDEST}"

support/scripts/copy_datafiles.sh "${NPDEST}"
support/scripts/copy_docfiles.sh "${NPDEST}"

cp /usr/local/bin/SDL.dll "${NPDEST}"
cp /usr/local/lib/*.dll "${NPDEST}"
cp /usr/local/bin/README-SDL.txt "${NPDEST}"
cp /usr/local/bin/README-SDL_mixer.txt "${NPDEST}"

[ ! -d "${RELEASEDIR}" ] && mkdir "${RELEASEDIR}"

pushd "${NPDEST}/.."
zip -9r "${ZIPNAME}" `basename "${NPDEST}"`
popd

[ -f "${RELEASEDIR}/${ZIPNAME}" ] && rm -f "${RELEASEDIR}/${ZIPNAME}"

mv "${NPDEST}/../${ZIPNAME}" "${RELEASEDIR}"
rm -rf "${NPDEST}"

