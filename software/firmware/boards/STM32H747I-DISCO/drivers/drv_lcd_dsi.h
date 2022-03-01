/*
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 * All rights reserved.
 * Change Logs:
 * Date           Author       Notes
 * 2020-07-01     RT-Thread    the first version
 */

#ifndef __DRV_LCD_DSI_H__
#define __DRV_LCD_DSI_H__
#include "stm32h7xx.h"
  

/* LCD Size (Width and Height) */
#define  LCD_PIXEL_WIDTH          (800)
#define  LCD_PIXEL_HEIGHT         (480)
#define  LCD_BITS_PER_PIXEL        24
#define  LCD_DSI_BUF_SIZE         (LCD_PIXEL_WIDTH*LCD_PIXEL_HEIGHT*LCD_BITS_PER_PIXEL/8)
int rt_hw_lcd_init(void);

#endif  /* __DRV_LCD_DSI_H__ */
