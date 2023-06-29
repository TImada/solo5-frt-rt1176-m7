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

#include <assert.h>
#include "platform_tender.h"
#include "lwip/netif.h"
#include "fsl_clock.h"

extern struct netif solo5_netif[MAX_NETDEV];

/* Code from lwip_ping_freertos.c */
/** Variables **/
#if BOARD_NETWORK_USE_100M_ENET_PORT
phy_ksz8081_resource_t g_phy_resource;
#else
phy_rtl8211f_resource_t g_phy_resource;
#endif

phy_handle_t phyHandle;
ethernetif_config_t enet_config;
uint32_t SystickClock = 0U;

/** BOARD_InitModuleClock **/
void BOARD_InitModuleClock(void)
{
    const clock_sys_pll1_config_t sysPll1Config = {
        .pllDiv2En = true,
    };
    CLOCK_InitSysPll1(&sysPll1Config);

#if BOARD_NETWORK_USE_100M_ENET_PORT
    clock_root_config_t rootCfg = {.mux = 4, .div = 10}; /* Generate 50M root clock. */
    CLOCK_SetRootClock(kCLOCK_Root_Enet1, &rootCfg);
#else
    clock_root_config_t rootCfg = {.mux = 4, .div = 4}; /* Generate 125M root clock. */
    CLOCK_SetRootClock(kCLOCK_Root_Enet2, &rootCfg);
#endif
}

/** IOMUXC_SelectENETClock **/
void IOMUXC_SelectENETClock(void)
{
#if BOARD_NETWORK_USE_100M_ENET_PORT
    IOMUXC_GPR->GPR4 |= IOMUXC_GPR_GPR4_ENET_REF_CLK_DIR_MASK; /* 50M ENET_REF_CLOCK output     to PHY and ENET module. */
#else
    IOMUXC_GPR->GPR5 |= IOMUXC_GPR_GPR5_ENET1G_RGMII_EN_MASK; /* bit1:iomuxc_gpr_enet_clk_di    r
                                                                 bit0:GPR_ENET_TX_CLK_SEL(in    ternal or OSC) */
#endif
}

/** BOARD_ENETFlexibleConfigure **/
void BOARD_ENETFlexibleConfigure(enet_config_t *config)
{
#if BOARD_NETWORK_USE_100M_ENET_PORT
    config->miiMode = kENET_RmiiMode;
#else
    config->miiMode = kENET_RgmiiMode;
#endif
}

/** MDIO_Init **/
static void MDIO_Init(void)
{
    (void)CLOCK_EnableClock(s_enetClock[ENET_GetInstance(EXAMPLE_ENET)]);
    ENET_SetSMI(EXAMPLE_ENET, EXAMPLE_CLOCK_FREQ, false);
}

/** MDIO_Write **/
static status_t MDIO_Write(uint8_t phyAddr, uint8_t regAddr, uint16_t data)
{
    return ENET_MDIOWrite(EXAMPLE_ENET, phyAddr, regAddr, data);
}

/** MDIO_Read **/
static status_t MDIO_Read(uint8_t phyAddr, uint8_t regAddr, uint16_t *pData)
{
    return ENET_MDIORead(EXAMPLE_ENET, phyAddr, regAddr, pData);
}

/* Initialization */
/* Early platform initialization before FreeRTOS becomes ready */
void early_platform_init_1(void)
{
    gpio_pin_config_t gpio_config = {kGPIO_DigitalOutput, 0, kGPIO_NoIntmode};
    uint32_t CpuClock = 0U;

    BOARD_ConfigMPU();
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    BOARD_InitModuleClock();

    IOMUXC_SelectENETClock();

#if BOARD_NETWORK_USE_100M_ENET_PORT
    BOARD_InitEnetPins();
    GPIO_PinInit(GPIO12, 12, &gpio_config);
    GPIO_WritePinOutput(GPIO12, 12, 0);
    SDK_DelayAtLeastUs(10000, CLOCK_GetFreq(kCLOCK_CpuClk));
    GPIO_WritePinOutput(GPIO12, 12, 1);
    SDK_DelayAtLeastUs(6, CLOCK_GetFreq(kCLOCK_CpuClk));
#else
    BOARD_InitEnet1GPins();
    GPIO_PinInit(GPIO11, 14, &gpio_config);
    /* For a complete PHY reset of RTL8211FDI-CG, this pin must be asserted low
     * for at least 10ms. And 276wait for a further 30ms(for internal circuits 
     * settling time) before accessing the PHY register.
     */
    GPIO_WritePinOutput(GPIO11, 14, 0);
    SDK_DelayAtLeastUs(10000, CLOCK_GetFreq(kCLOCK_CpuClk));
    GPIO_WritePinOutput(GPIO11, 14, 1);
    SDK_DelayAtLeastUs(30000, CLOCK_GetFreq(kCLOCK_CpuClk));

    EnableIRQ(ENET_1G_MAC0_Tx_Rx_1_IRQn);
    EnableIRQ(ENET_1G_MAC0_Tx_Rx_2_IRQn);
#endif

    MDIO_Init();
    g_phy_resource.read  = MDIO_Read;
    g_phy_resource.write = MDIO_Write;

    /* Configure the target enet device */
    enet_config.phyHandle   = &phyHandle;
    enet_config.phyAddr     = EXAMPLE_PHY_ADDRESS;
    enet_config.phyOps      = EXAMPLE_PHY_OPS;
    enet_config.phyResource = EXAMPLE_PHY_RESOURCE;
    enet_config.srcClockHz  = EXAMPLE_CLOCK_FREQ;
#ifdef configMAC_ADDR
    enet_config.macAddress  = configMAC_ADDR;
#else
    (void)SILICONID_ConvertToMacAddr(&enet_config.macAddress);
#endif

    /* The enet_config address will be registered with solo5_netif[0].state 
     * in solo5_netif_add().
     */
    solo5_netif[0].state = (void *)&enet_config;
    
    /* Get the systick clock value */
    SystickClock = CLOCK_GetRootClockFreq(kCLOCK_Root_M7_Systick);

    /* Get the CPU core clock value */
    CpuClock = CLOCK_GetM7Freq();
    PLATFORM_PRINTF("i.MXRT1176 Cortex-M7 @ %u MHz\r\n", CpuClock / 1000000U);

    return;
}

/* Early platform initialization after FreeRTOS became ready */
void early_platform_init_2(void)
{
    return;
}

/* Timer */
/* Get the timer count */
#define SYSTICK_CURVAL (*((volatile uint32_t *)0xe000e018))
uint64_t platform_get_timer_count(void)
{
    /* Use the 24-bit systick counter */
    return (uint64_t)SYSTICK_CURVAL;
}

/* 
 * Timer count adjustment:
 * This function can be useful if the target timer device employs a 
 * specialized counting scheme.
 * Usually, this function should return the argument itself.
 */
uint64_t platform_adjusted_tcount(uint64_t tc)
{
    return tc;
}

