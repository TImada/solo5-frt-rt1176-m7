#! /bin/sh

# 'debug' or 'release'
BUILD=debug

prefix=${1:-$PREFIX}
if [ "$prefix" = "" ]; then
    prefix=`opam var prefix`
fi

DESTDIR=${prefix}/solo5-frt-rt1176-m7
mkdir -p ${DESTDIR}
mkdir ${DESTDIR}/include ${DESTDIR}/lib ${DESTDIR}/patch

# Copy the SoC specific Makefile
cp Makefile.soc Makefile.common ${DESTDIR}

# Copy the linker file
cp solo5_frt.lds ${DESTDIR}

# Copy header files
cp FreeRTOSConfig.h platform_*.h ${DESTDIR}/include

# Copy the library files
cp libboard_${BUILD}.a ${DESTDIR}/lib
cp libdriver_${BUILD}.a ${DESTDIR}/lib
cp libfreertos_${BUILD}.a ${DESTDIR}/lib
cp liblwip_${BUILD}.a ${DESTDIR}/lib
cp libsolo5_frt_${BUILD}.a ${DESTDIR}/lib

