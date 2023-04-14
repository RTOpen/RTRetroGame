/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#include <rtthread.h>
#include "board.h"

int platform_init(void)
{
#ifdef RT_USING_LWIP
#ifdef RT_USING_TAPNETIF
    tap_netif_hw_init();
#else
    pcap_netif_hw_init();
#endif
#endif

    return 0;
}
INIT_COMPONENT_EXPORT(platform_init);

