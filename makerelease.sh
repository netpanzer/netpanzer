#!/bin/sh
# a little script for making a netpanzer release

VERSION=`autoconf -t AC_INIT | sed -e 's/[^:]*:[^:]*:[^:]*:[^:]*:\([^:]*\):.*/\1/g'`

echo "Creating release netpanzer-$VERSION"

SOURCERELEASE=release/netpanzer-$VERSION
DATARELEASE=release/netpanzer-data-$VERSION

# first clean the directories up
echo "*** Cleaning up"
#if test ! -f configure; then
    ./autogen.sh
#fi
#if test ! -f Jamconfig; then
    ./configure
#fi
rm -rf out
rm -rf release
jam docbook
jam distclean

echo "*** Scanning for files"
AUTOFILES="autogen.sh configure.ac configure config.h.in `find mk/autoconf/ -name "*.m4" -o -name "config.*"` mk/autoconf/install-sh"
JAMFILES="Jamrules Jamconfig.in `find mk/jam -name "*.jam"`"
ICONS="*.png *.xpm"
DESKTOPFILES="netpanzer.desktop"
TEXTS="COPYING README TODO RELNOTES ChangeLog"
SOURCES="`find src -name "*.cpp" -o -name "*.hpp" -o -name "Jamfile"`"
DOCS="docs/*.[1-9] docs/serverhowto.html docs/tipofday.txt docs/Jamfile"

echo "*** Creating Sourcepackage"
mkdir -p $SOURCERELEASE
# Create a new Jamfile
echo "NO_DATA = 1 ;" > $SOURCERELEASE/Jamfile
cat Jamfile >> $SOURCERELEASE/Jamfile

cp -p --parents $AUTOFILES $SOURCERELEASE
cp -p --parents $JAMFILES $SOURCERELEASE
cp -p --parents $TEXTS $SOURCERELEASE
cp -p --parents $SOURCES $SOURCERELEASE
cp -p --parents $ICONS $SOURCERELEASE
cp -p --parents $DESKTOPFILES $SOURCERELEASE
cp -p --parents $DOCS $SOURCERELEASE

echo "*** Packing source"
cd release
tar -c --bzip2 -f netpanzer-$VERSION.tar.bz2 netpanzer-$VERSION
cd -

echo "*** Creating data release"
mkdir -p $DATARELEASE
# Create a new Jamfile
echo "NO_SRC = 1 ;" > $DATARELEASE/Jamfile
cat Jamfile >> $DATARELEASE/Jamfile

find pics \( -name "*.bmp" -o -name "*.raw" -o -name "*.til" -o -name "*.pak" -o  -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
find wads \( -name "*.act" -o -name "*.tls" -o -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
find powerups \( -name "*.pak" -o -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
find units \( -name "*.pfl" -o -name "*.pak" -o -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
find sound \( -name "*.wav" -o -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
find maps \( -name "*.npm" -o -name "*.opt" -o -name "*.spn" -o -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
find cache \( -name "*.tbl" -o -name "Jamfile" \) -exec cp -p --parents {} $DATARELEASE ';'
# find fonts \( -name "*.pcf" \) -exec cp -p --parents {} $DATARELEASE ';'
cp -p --parents $AUTOFILES $DATARELEASE
cp -p --parents $JAMFILES $DATARELEASE
cp -p --parents $TEXTS $DATARELEASE

echo "*** Packing Data"
cd release
tar -c --bzip2 -f netpanzer-data-$VERSION.tar.bz2 netpanzer-data-$VERSION
cd -

