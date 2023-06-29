#! /bin/sh

prefix=${1:-$PREFIX}
if [ "$prefix" = "" ]; then
    prefix=`opam var prefix`
fi

DESTDIR=${prefix}/solo5-frt-rt1176-m7

# Remove the target directory
rm -rf ${DESTDIR}

