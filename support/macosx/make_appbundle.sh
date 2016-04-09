#! /bin/bash

if [ ! -f RELEASE_VERSION ]; then
    echo "ERROR: RELEASE_VERSION file missing, cannot create"
    exit 1
fi

VERSION="`<RELEASE_VERSION`"
RELEASEDIR="releases"
APPNAME="NetPanzer"
BASEDMG="support/macosx/basedmg.dmg"
TEMPDMG="${RELEASEDIR}/tempdmg.dmg"
FINALDMG="${RELEASEDIR}/NetPanzer-Mac-Universal-${VERSION}.dmg"
DMGROOT="${RELEASEDIR}/MacOS_DMGROOT"
DMGDOC="${DMGROOT}/Documentation"
APPDIR="${DMGROOT}/${APPNAME}.app"
CONTENTSDIR="${APPDIR}/Contents"

echo "Compiling Mac version ${VERSION}"

scons universal=true

echo "Cleaning work folder"
[ ! -d ${RELEASEDIR} ] && mkdir ${RELEASEDIR}

rm -f ${TEMPDMG}
rm -f ${FINALDMG}

echo "Uncompressing and mounting dmg"
hdiutil convert "${BASEDMG}" -format UDRW -o "${TEMPDMG}"
hdiutil attach "${TEMPDMG}" -mountpoint "${DMGROOT}"

mkdir "${CONTENTSDIR}" "${CONTENTSDIR}"/{Frameworks,MacOS,Resources}

support/scripts/copy_datafiles.sh "${CONTENTSDIR}/Resources"
support/scripts/copy_docfiles.sh "${DMGDOC}"

cp support/icon/netpanzer.icns "${CONTENTSDIR}/Resources"
echo -n "."
cp netpanzer "${CONTENTSDIR}/MacOS/"
echo "."

echo "Copying needed frameworks..."
cp -R /Library/Frameworks/SDL{,_mixer}.framework "${CONTENTSDIR}/Frameworks"

echo "Creating info files..."

echo -n 'APPL????' > "${CONTENTSDIR}/PkgInfo"

cat << EOF > "${CONTENTSDIR}/Info.plist"
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>CFBundleDevelopmentRegion</key>
	<string>English</string>
	<key>CFBundleExecutable</key>
	<string>netpanzer</string>
	<key>CFBundleIconFile</key>
	<string>netpanzer.icns</string>
	<key>CFBundleInfoDictionaryVersion</key>
	<string>6.0</string>
	<key>CFBundlePackageType</key>
	<string>APPL</string>
	<key>CFBundleSignature</key>
	<string>????</string>
	<key>CFBundleVersion</key>
	<string>0.0.1d1</string>
	<key>CFBundleURLTypes</key>
	<array>
		<dict>
			<key>CFBundleURLName</key>
			<string>NetPanzer game URL</string>
			<key>CFBundleURLSchemes</key>
			<array>
				<string>netpanzer</string>
			</array>
		</dict>
	</array>
</dict>
</plist>
EOF

echo "Unmounting and creating final DMG"
hdiutil detach "${DMGROOT}" -force
hdiutil resize -sectors min "${TEMPDMG}"
hdiutil convert "${TEMPDMG}" -format UDZO -o "${FINALDMG}" -imagekey zlib-level=9
rm -f "${TEMPDMG}"
