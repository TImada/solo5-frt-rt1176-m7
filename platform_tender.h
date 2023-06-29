/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2020,2022 NXP
 * Copyright (c) 2023 Takayuki Imada <takayuki.imada@gmail.com>
 * All rights reserved.
 *
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Part of this file is derived from lwip_ping_freertos.c
 */

#define _GNU_SOURCE

/* Code from lwip_ping_freertos.c */
#include "pin_mux.h"
#include "board.h"
#include "fsl_debug_console.h"
#include "ethernetif.h"
#ifndef configMAC_ADDR
#include "fsl_silicon_id.h"
#endif
#include "fsl_phy.h"

#if BOARD_NETWORK_USE_100M_ENET_PORT
#include "fsl_phyksz8081.h"
#else
#include "fsl_phyrtl8211f.h"
#endif
#include "fsl_enet.h"

/* Macros for networking (ENET) */
#if BOARD_NETWORK_USE_100M_ENET_PORT
#define EXAMPLE_ENET ENET
/** Address of PHY interface. **/
#define EXAMPLE_PHY_ADDRESS BOARD_ENET0_PHY_ADDRESS
/** PHY operations. **/
#define EXAMPLE_PHY_OPS &phyksz8081_ops
/** ENET instance select. **/
#define EXAMPLE_NETIF_INIT_FN ethernetif0_init
/** PHY resource data. **/
extern phy_ksz8081_resource_t g_phy_resource;
#else
#define EXAMPLE_ENET          ENET_1G
/** Address of PHY interface. **/
#define EXAMPLE_PHY_ADDRESS   BOARD_ENET1_PHY_ADDRESS
/** PHY operations. **/
#define EXAMPLE_PHY_OPS       &phyrtl8211f_ops
/** ENET instance select. **/
#define EXAMPLE_NETIF_INIT_FN ethernetif1_init
/** PHY resource data. **/
extern phy_rtl8211f_resource_t g_phy_resource;
#endif /* BOARD_NETWORK_USE_100M_ENET_PORT */

/* PHY resource. */
#define EXAMPLE_PHY_RESOURCE &g_phy_resource

/* ENET clock frequency. */
#define EXAMPLE_CLOCK_FREQ CLOCK_GetRootClockFreq(kCLOCK_Root_Bus)

/******************************************************************************/
/* Base task priority for MirageOS related tasks */
#define MIRAGE_BASE_PRI (3)

/* Macros for the console */
#define PLATFORM_PRINTF PRINTF
#define PLATFORM_PERROR PLATFORM_PRINTF

/* Macros for networking (LWIP) */
#define LWIP_INPUT_TASK
#define LWIP_NETIF_INIT EXAMPLE_NETIF_INIT_FN /* RT1176 ENET init handler */

/* Supported number of each I/O device type */
#define MAX_NETDEV (1U) /* Currently support only one network device */

/* Initialization */
/* Early platform initialization before FreeRTOS becomes ready */
void early_platform_init_1(void);
/* Early platform initialization after FreeRTOS became ready */
void early_platform_init_2(void);

/* Timer */
/* Get the timer count */
uint64_t platform_get_timer_count(void);

/* Timer count adjustment */
uint64_t platform_adjusted_tcount(uint64_t tc);
