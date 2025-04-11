/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */
#include <rtthread.h>
#include <stdio.h>
#include <board.h>

int main(void)
{
    rt_kprintf("Hello RT-Thread!\n");
    return 0;
}

void lv_user_gui_init(void)
{
    /* display demo; you may replace with your LVGL application at here */
    extern void ui_splash_init(void);
    extern void launcher_init(void);
    //ui_splash_init();
    launcher_init();
    //ui_init();
}