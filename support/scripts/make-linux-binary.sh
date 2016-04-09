#! /bin/bash

if [ ! -f RELEASE_VERSION ]; then
    echo "ERROR: RELEASE_VERSION file missing, cannot create"
    exit 1
fi

RELEASEDIR="releases"
VERSION="`<RELEASE_VERSION`"
NPDEST=/tmp/netpanzer
ZIPNAME="netpanzer-linux-${VERSION}.tar.gz"
EXENAME="build/crosslinux/release/netpanzer"

PATH=$PATH:/usr/local/gcc/i686-linux/bin/

echo "Making netPanzer linux version ${VERSION}"

mkdir -p build/crosslinux/{release,debug}/libs
ln -s "`i686-linux-g++ -print-file-name=libstdc++.a`" build/crosslinux/release/libs/libstdc++.a
ln -s "`i686-linux-g++ -print-file-name=libstdc++.a`" build/crosslinux/debug/libs/libstdc++.a

scons crosslinux \
      crosslinuxcompilerprefix=i686-linux- \
      crosslinuxsdlconfig=/usr/local/gcc/i686-linux/bin/sdl-config

i686-linux-strip "${EXENAME}"

[ -d "${NPDEST}" ] && rm -rf "${NPDEST}"

mkdir -p "${NPDEST}"
cp "${EXENAME}" "${NPDEST}"

support/scripts/copy_datafiles.sh "${NPDEST}"
support/scripts/copy_docfiles.sh "${NPDEST}"

[ ! -d "${RELEASEDIR}" ] && mkdir "${RELEASEDIR}"


find "${NPDEST}" -name "*/.*" -exec rm -f "{}" \;

pushd "${NPDEST}/.."
tar zcvf "${ZIPNAME}" `basename "${NPDEST}"`
popd

[ -f "${RELEASEDIR}/${ZIPNAME}" ] && rm -f "${RELEASEDIR}/${ZIPNAME}"

mv "${NPDEST}/../${ZIPNAME}" "${RELEASEDIR}"
rm -rf "${NPDEST}"
