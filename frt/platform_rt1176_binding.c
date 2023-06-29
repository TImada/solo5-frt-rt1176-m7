/*
 * Copyright (c) 2023 Takayuki Imada <takayuki.imada@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define _GNU_SOURCE

#include "FreeRTOS.h"
#include "task.h"
#include "fsl_debug_console.h"
#include "ethernetif.h"
#include "lwip/netifapi.h"
#include "lwip/sys.h"
#include "lwip/tcpip.h"
#include "platform_tender.h"

extern struct netif solo5_netif[MAX_NETDEV];

#define NWS_TASK_PRI (1)
#define NWS_STACK_SIZE (256)
#define NWS_STACK_ELEMS ((NWS_STACK_SIZE)/sizeof(configSTACK_DEPTH_TYPE))
StackType_t nwsStack[NWS_STACK_ELEMS] __attribute__((section(".nws_stack"), aligned(64)));
TaskHandle_t nwsTask = NULL;
static StaticTask_t nwsTaskObj;

static void nw_status_handler(void *args)
{
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        vTaskDelayUntil(&xLastWakeTime, (1000U / portTICK_PERIOD_MS));
        ethernetif_probe_link(&solo5_netif[0]);
    }

    return;
}

/* Initialization */
/* CPU initialization */
void platform_cpu_init(void)
{
    return;
}

/* Timer initialization */
void platform_timer_init(void)
{
    return;
}

/* Console initialization */
void platform_console_init(void)
{
    return;
}

/* Block device initialization */
void platform_block_init(void)
{
    return;
}

/* Network device initialization */
void platform_net_init(void)
{
#if LWIP_TCPIP_CORE_LOCKING
    if (sys_mutex_new(&lock_tcpip_core) != ERR_OK) {
      LWIP_ASSERT("Failed to create lock_tcpip_core", 0);
    }
#endif /* LWIP_TCPIP_CORE_LOCKING */
    
    /* Create a new task to detect network status periodically */
    nwsTask = xTaskCreateStatic(nw_status_handler,
                                "nw_status",
                                NWS_STACK_ELEMS,
                                NULL,
                                NWS_TASK_PRI,
                                nwsStack,
                                &nwsTaskObj);
    if (nwsTask == NULL) {
        LWIP_ASSERT("Failed to create a task to check network status", 0);
    }
 
    /* Wait until the port becomes ready */
    netifapi_netif_set_default(&solo5_netif[0]);
    netifapi_netif_set_up(&solo5_netif[0]);
    while (ethernetif_wait_linkup(&solo5_netif[0], 5000) != ERR_OK)
    {
         DbgConsole_Printf("solo5_netif[%d] : Auto-negotiation failed.\r\n", 0);
    }

    return;
}

/* Console */
int platform_puts(const char *buf, int n)
{
    /* TODO:
     * We do not use DbgConsole_Printf to insert '\r' before '\n'.
     * Too primitive implementation, this should be replaced
     * by a smarter scheme.
     */
    int i;

    for(i = 0; i < n; i++) {
        switch(buf[i]){
            case '\n':
                /* Insert '\r' internally before '\n' */
                DbgConsole_Putchar('\r');
                DbgConsole_Putchar('\n');
                break;
            default:
                DbgConsole_Putchar(buf[i]);
                break;
        }
    }
    return n;
}
