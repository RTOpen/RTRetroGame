/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-03-05     whj4674672   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include "drv_led.h"


int main(void)
{
    uint32_t loop = 0;

    while (1)
    {
			if((loop%2)==0)
			{
      led_toggle(LED_GREEN);
			}
			if((loop%4)==0)
			{
      led_toggle(LED_YELLOW);
			}
			if((loop%8)==0)
			{
      led_toggle(LED_RED);
			}
			if((loop%16)==0)
			{
      led_toggle(LED_BLUE);
			}
			loop++;
			rt_thread_mdelay(100);
    }
}
