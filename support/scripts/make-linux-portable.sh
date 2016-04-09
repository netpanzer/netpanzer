#! /bin/bash

if [ ! -f RELEASE_VERSION ]; then
    echo "ERROR: RELEASE_VERSION file missing, cannot create"
    exit 1
fi

RELEASEDIR="releases"
VERSION="`<RELEASE_VERSION`"
NPDEST=/tmp/netpanzer
ZIPNAME="netpanzer-linux-${VERSION}.tar.gz"
EXENAME="netpanzer"

echo "Making netPanzer linux portable version ${VERSION}"

mkdir -p build/{release,debug}/libs
ln -s "`g++ -print-file-name=libstdc++.a`" build/release/libs/libstdc++.a
ln -s "`g++ -print-file-name=libstdc++.a`" build/debug/libs/libstdc++.a

scons

strip "${EXENAME}"

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
