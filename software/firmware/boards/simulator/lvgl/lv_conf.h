/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author        Notes
 * 2021-10-18     Meco Man      First version
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <rtconfig.h>

#define LV_USE_PERF_MONITOR         1
#define LV_COLOR_DEPTH              32
#define LV_HOR_RES_MAX              BSP_LCD_WIDTH
#define LV_VER_RES_MAX              BSP_LCD_HEIGHT

#define USE_WIN32DRV                1
#define WIN32DRV_MONITOR_ZOOM       1

#define LV_FONT_MONTSERRAT_8        1
#define LV_FONT_MONTSERRAT_12       1
#define LV_FONT_MONTSERRAT_16       1
#define LV_FONT_MONTSERRAT_20       1
#define LV_FONT_MONTSERRAT_26       1
#define LV_FONT_MONTSERRAT_34       1
#define LV_COLOR_SCREEN_TRANSP      1

#define LV_USE_GIF                  1
#define LV_USE_PNG                  1
#define LV_USE_BMP                  1

#define LV_IMG_CACHE_DEF_SIZE       8192
#endif
