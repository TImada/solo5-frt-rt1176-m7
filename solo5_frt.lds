/*
** ###################################################################
**     Processors:          MIMXRT1176AVM8A_cm7
**                          MIMXRT1176CVM8A_cm7
**                          MIMXRT1176DVMAA_cm7
**
**     Abstract:
**         Linker file for the GNU C Compiler
**
**     Copyright 2016 Freescale Semiconductor, Inc.
**     Copyright 2016-2022 NXP
**     Copyright 2023 Takayuki Imada <takayuki.imada@gmail.com>
**     All rights reserved.
**
**     SPDX-License-Identifier: BSD-3-Clause
**
**     This file is derived from MIMXRT1176xxxxx_cm7_sdram.ld in
**     the LWIP ping example for CM7.
**
** ###################################################################
*/

/* Entry Point */
ENTRY(Reset_Handler)

HEAP_SIZE  = DEFINED(__heap_size__)  ? __heap_size__  : 25600;
STACK_SIZE = DEFINED(__stack_size__) ? __stack_size__ : 2048;

/* SDRAM starts at 0x80000000.
 * But the first 8KB(0x2000) reserved for the bootloader.
 */
SDRAM_START         = 0x80002000;
SDRAM_SIZE          = 0x03FFE000; /* 64MB - 8KB */
SDRAM_CODE_START    = SDRAM_START;
SDRAM_CODE_SIZE     = 0x003FE000; /* 4MB */
SDRAM_DATA_START    = SDRAM_CODE_START + SDRAM_CODE_SIZE;
SDRAM_DATA_SIZE     = 0x00400000; /* 4MB */
SDRAM_NCACHE_START  = SDRAM_DATA_START + SDRAM_DATA_SIZE;
SDRAM_NCACHE_SIZE   = 0x00200000; /* 2MB */
MIRAGE_HEAP_START   = SDRAM_NCACHE_START + SDRAM_NCACHE_SIZE;
MIRAGE_HEAP_SIZE    = 0x01000000; /* 16MB */

/* Specify the memory areas */
MEMORY
{
  /* 512KB ITCM for isr_vector */
  ITCM          (RX)  : ORIGIN = 0x00002000, LENGTH = 0x0007C000
  /* 512KB DTCM (not used) */
  DTCM          (RW)  : ORIGIN = 0x20000000, LENGTH = 0x00080000
  /* 2x 512KB OCRAMs (not used) */
  OCRAM1        (RW)  : ORIGIN = 0x20240000, LENGTH = 0x00080000
  OCRAM2        (RW)  : ORIGIN = 0x202C0000, LENGTH = 0x00080000
  /* Code region */
  SDRAM_CODE    (RX)  : ORIGIN = SDRAM_CODE_START, LENGTH = SDRAM_CODE_SIZE
  /* Data (cacheable) reagion */
  SDRAM_DATA    (RW)  : ORIGIN = SDRAM_DATA_START, LENGTH = SDRAM_DATA_SIZE
  /* Data (non-cacheable) reagion */
  SDRAM_NCACHE  (RW)  : ORIGIN = SDRAM_NCACHE_START, LENGTH = SDRAM_NCACHE_SIZE
  /* MirageOS heap region */
  MIRAGE_HEAP   (RW)  : ORIGIN = MIRAGE_HEAP_START, LENGTH = MIRAGE_HEAP_SIZE
}

/* Define output sections */
SECTIONS
{
  __NCACHE_REGION_START = ORIGIN(SDRAM_NCACHE);
  __NCACHE_REGION_SIZE  = LENGTH(SDRAM_NCACHE);

  /* The startup code goes first into internal RAM */
  .interrupts :
  {
    __VECTOR_TABLE = .;
    __Vectors = .;
    . = ALIGN(4);
    KEEP(*(.isr_vector))     /* Startup code */
    . = ALIGN(4);
  } > SDRAM_CODE

  _mem_start = SDRAM_START;
  _mem_size = SDRAM_SIZE;

  /* The program code and other data goes into internal RAM */
  _stext = _mem_start;

  .text :
  {
    . = ALIGN(4);
    *(.text)                 /* .text sections (code) */
    *(.text*)                /* .text* sections (code) */
    _etext = .;
    *(.rodata)               /* .rodata sections (constants, strings, etc.) */
    *(.rodata*)              /* .rodata* sections (constants, strings, etc.) */
    *(CodeQuickAccess)       /* quick access code section */
    *(.glue_7)               /* glue arm to thumb code */
    *(.glue_7t)              /* glue thumb to arm code */
    *(.eh_frame)
    KEEP (*(.init))
    KEEP (*(.fini))
    . = ALIGN(4);
  } > SDRAM_CODE

  .ARM.extab :
  {
    *(.ARM.extab* .gnu.linkonce.armextab.*)
  } > SDRAM_CODE

  .ARM :
  {
    __exidx_start = .;
    *(.ARM.exidx*)
    __exidx_end = .;
  } > SDRAM_CODE

 .ctors :
  {
    __CTOR_LIST__ = .;
    /* gcc uses crtbegin.o to find the start of
       the constructors, so we make sure it is
       first.  Because this is a wildcard, it
       doesn't matter if the user does not
       actually link against crtbegin.o; the
       linker won't look for a file to match a
       wildcard.  The wildcard also means that it
       doesn't matter which directory crtbegin.o
       is in.  */
    KEEP (*crtbegin.o(.ctors))
    KEEP (*crtbegin?.o(.ctors))
    /* We don't want to include the .ctor section from
       from the crtend.o file until after the sorted ctors.
       The .ctor section from the crtend file contains the
       end of ctors marker and it must be last */
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
    __CTOR_END__ = .;
  } > SDRAM_CODE

  .dtors :
  {
    __DTOR_LIST__ = .;
    KEEP (*crtbegin.o(.dtors))
    KEEP (*crtbegin?.o(.dtors))
    KEEP (*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors))
    KEEP (*(SORT(.dtors.*)))
    KEEP (*(.dtors))
    __DTOR_END__ = .;
  } > SDRAM_CODE

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > SDRAM_CODE

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > SDRAM_CODE

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP (*(SORT(.fini_array.*)))
    KEEP (*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > SDRAM_CODE

  _erodata = .;
  __etext = .;    /* define a global symbol at end of code */
  __DATA_ROM = .; /* Symbol is used by startup for data initialization */

  __VECTOR_RAM = ORIGIN(ITCM);
  __RAM_VECTOR_TABLE_SIZE_BYTES = 0x0;

  .data : AT(__DATA_ROM)
  {
    . = ALIGN(4);
    __DATA_RAM = .;
    __data_start__ = .;      /* create a global symbol at data start */
    __start_modules = .;
    *(modules)               /* Solo5 modules */
    __stop_modules = .;
    *(iodevs)                /* Solo5 frt I/O devices */
    *(m_usb_dma_init_data)
    *(.data)                 /* .data sections */
    *(.data*)                /* .data* sections */
    KEEP(*(.jcr*))
    . = ALIGN(4);
    __data_end__ = .;        /* define a global symbol at data end */
  } > SDRAM_DATA

  __NDATA_ROM = __DATA_ROM + (__data_end__ - __data_start__);
  .ncache.init : AT(__NDATA_ROM)
  {
    __noncachedata_start__ = .;   /* create a global symbol at ncache data start */
    *(NonCacheable.init)
    . = ALIGN(4);
    __noncachedata_init_end__ = .;   /* create a global symbol at initialized ncache data end */
  } > SDRAM_NCACHE
  . = __noncachedata_init_end__;
  .ncache :
  {
    *(NonCacheable)
    . = ALIGN(4);
    __noncachedata_end__ = .;     /* define a global symbol at ncache data end */
  } > SDRAM_NCACHE

  .qadata :
  {
    . = ALIGN(4);
    *(DataQuickAccess)       /* quick access data section */
    . = ALIGN(4);
  } > SDRAM_DATA

  _edata = .;

  /* Uninitialized data section */
  .bss :
  {
    /* This is used by the startup in order to initialize the .bss section */
    . = ALIGN(4);
    __START_BSS = .;
    __bss_start__ = .;
    *(m_usb_dma_noninit_data)
    *(.bss)
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;
    __END_BSS = .;
  } > SDRAM_DATA

  _ebss = .;

  .stack :
  {
    . = ALIGN(8);
    . += STACK_SIZE;
    __StackTop = .;
  } > SDRAM_DATA
  __StackLimit = __StackTop - STACK_SIZE;
  PROVIDE(__stack = __StackTop);

  _estack = .;

  .heap :
  {
    . = ALIGN(8);
    __end__ = .;
    PROVIDE(end = .);
    __HeapBase = .;
    . += HEAP_SIZE;
    __HeapLimit = .;
    __heap_limit = .; /* Add for _sbrk */
  } > SDRAM_DATA

  _efheap = .;
  _ekernel = .;

  .mirage_heap (NOLOAD) : {
    _mirage_heap_start = .;
    KEEP(*(.mirage_heap))
    . = . + MIRAGE_HEAP_SIZE;
    _mirage_heap_end = .;
  } > MIRAGE_HEAP
  . = ALIGN (8);
  _emheap = .;

  .ARM.attributes 0 : { *(.ARM.attributes) }
}
