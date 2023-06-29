# How to build your MirageOS unikernel

This document describes how to build your MirageOS unikernel running on top of the Cortex(r)-M7 core of your iMX RT1176 processor.

## Prerequisites

### Board

This repository was tested with a MIMXRT1170-EVK board provided by NXP.

(i.MX RT1170 Evaluation Kit)
URL: https://www.nxp.jp/design/development-boards/i-mx-evaluation-and-development-boards/i-mx-rt1170-evaluation-kit:MIMXRT1170-EVK

### Install SDKs for MIMXRT1170-EVK

You need to install MCUXpresso Software Development Kit (SDK) on your Linux PC.

URL: https://www.nxp.jp/design/software/development-software/mcuxpresso-software-and-tools-/mcuxpresso-software-development-kit-sdk:MCUXpresso-SDK

You will use MCUXpresso SDK Builder to create a zipped SDK file. Use the following build configuration.

```
Target board      : MIMXRT1170-EVK
SDK version       : v2.13.1
Packages included : SDMMC Stack, lwIP, FreeRTOS
```

### Install GCC 10  

URL: https://developer.arm.com/downloads/-/gnu-rm  
Get the the version 10.3-2021.10 (gcc-arm-none-eabi-10.3-2021.10).  

### Set variables on your bash

```bash
# SDK directory
$ export MCU_SDK_TOP=/path/to/SDK_2_13_1_MIMXRT1170-EVK

# GCC directory
$ export ARMGCC_DIR=/path/to/gcc-arm-none-eabi-10.3-2021.10
```

## Register a repository for MCUs with opam

Follow [this instruction](https://github.com/TImada/mirage-embedded-repo/blob/main/HOWTO.md).

## Solo5/MirageOS installation

```bash
# Install the mirage package
$ opam install mirage

# Install the solo5-frt-rt1176-m7 package for your target processor (target processor: "rt1176-m7")
$ opam install solo5-frt-rt1176-m7

# Install the Solo5 frt configuration package to select the processor "rt1176-m7" as a target processor for the Solo5 frt
$ opam install conf-frt.rt1176-m7
```

## Build your MirageOS unikernel

Finally, you are now ready to build your MirageOS unikernel. Let's build the hello world console sample!

```bash
# Get a collection of MirageOS unikernel samples
$ git clone https://github.com/mirage/mirage-skeleton
$ cd ./mirage-skeleton/device-usage/console

# Configure MirageOS so that it can use the Solo5 frt
$ mirage configure -t frt

# Compile the target unikernel (taking long time if this is the first compilation)
$ make
```

