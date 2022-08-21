/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-05     xiaonong      first implementation
 */
#ifndef __BOARD_H__
#define __BOARD_H__
#include "rtthread.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"

/* board configuration */
#define HEAP_BEGIN       0x24000000

// <o> Internal SRAM memory size[Kbytes] <8-64>
//  <i>Default: 64
#define STM32_SRAM_SIZE   (512 * 1024)
#define HEAP_END          (0x24000000 + STM32_SRAM_SIZE)

void rt_hw_board_init(void);

#endif

