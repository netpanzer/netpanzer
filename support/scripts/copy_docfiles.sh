#! /bin/bash

if [ -z "$1" ]; then
	echo "Error: missing destination, use: $0 <destination/dir>"
	exit 1
fi

DESTDIR="$1"

[ ! -d "${DESTDIR}" ] && mkdir -p "${DESTDIR}"

echo -n "Copying files"
cp BUGS ChangeLog COPYING LICENSE-physfs.txt README README-zlib123.txt RELNOTES TODO "${DESTDIR}"
echo "."