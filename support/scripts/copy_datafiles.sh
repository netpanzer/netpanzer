#! /bin/bash

if [ -z "$1" ]; then
	echo "Error: missing destination, use: $0 <destination/dir>"
	exit 1
fi

DESTDIR="$1"

[ ! -d "${DESTDIR}" ] && mkdir -p "${DESTDIR}"

echo -n "Copying files"
find cache maps pics powerups scripts sound units wads ! -type d  ! -path '*/.svn/*' ! -iname '.DS_Store' -print | \
	while read fname; do 
		DNAME="`dirname \"${fname}\"`"
		[ ! -d "${DESTDIR}/${DNAME}" ] && mkdir -p "${DESTDIR}/${DNAME}"
		cp "${fname}" "${DESTDIR}/${fname}"
		echo -n "."
	done

echo "."
