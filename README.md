# solo5-frt-rt1176

This package provides a variety of files required to build a MirageOS unikernel with Solo5 running on top of a Cortex(r)-M7 core of your NXP i.MX RT1176 processor.

## How to build your MirageOS unikernel

Follow instructions described in [BUILD.md](./BUILD.md).

## What is done during installation of this package

(in GNUMakefile)

1. Build a FreeRTOS library file `libfreertos_debug.a`
2. Build a LWIP library file `liblwip_debug.a`
3. Build a driver library file `libdriver_debug.a`
4. Build a board library file `libboard_debug.a`
5. Build a board specific solo5 frt library file `libsolo5_frt_debug.a`

(in install.sh)

5. Copy `Makefile.soc`   and `solo5_frt.lds` to `$HOME/.opam/$(opam var prefix)/solo5-frt-r51176-m7`
6. Copy `FreeRTOSConfig.h`, `platform_binding.h`, and `platform_tender.h` to `$HOME/.opam/$(opam var prefix)/solo5-frt-rt1176-m7/include`
7. Copy the library files above to `$HOME/.opam/$(opam var prefix)/solo5-frt-rt1176-m7/lib`

## LICENSE

ISC lincence is applied as indicated in the LICENSE file unless noted in each file.

Exceptionally, one of other licneses are applied to some files.

### BSD-3-Clause license

- `solo5_frt.lds`
- `lwipopt.h`
- `platform_tender.h`
- `frt/platform_rt1176_tender.c`
- `rt1170_evk/board_solo5.c`

### MIT license

- `FreeRTOSConfig.h`
