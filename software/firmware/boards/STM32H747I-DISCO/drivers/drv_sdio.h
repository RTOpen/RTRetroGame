/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-23     xiaoming   first version
 */

#ifndef __DRV_SDIO_H__
#define __DRV_SDIO_H__
#include "stm32h7xx.h"
#include <rtthread.h>
#include "rtdevice.h"
#include <rthw.h>
#include <string.h>
#include <drivers/mmcsd_core.h>
#include <drivers/sdio.h>

#define SDIO_BUFF_SIZE       4096
#define SDIO_ALIGN_LEN       32

#ifndef SDIO1_BASE_ADDRESS
    #define SDIO1_BASE_ADDRESS    (0x52007000)
#endif

#ifndef SDIO2_BASE_ADDRESS
    #define SDIO2_BASE_ADDRESS    (0x48022400)
#endif

#ifndef SDIO_CLOCK_FREQ
    #define SDIO_CLOCK_FREQ      (200U * 1000 * 1000)
#endif

#ifndef SDIO_BUFF_SIZE
    #define SDIO_BUFF_SIZE       (4096)
#endif

#ifndef SDIO_ALIGN_LEN
    #define SDIO_ALIGN_LEN       (32)
#endif

#ifndef SDIO_MAX_FREQ
    #define SDIO_MAX_FREQ        (25 * 1000 * 1000)
#endif

#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))

#define SDIO_ERRORS \
    (SDMMC_STA_IDMATE | SDMMC_STA_ACKTIMEOUT | \
     SDMMC_STA_RXOVERR | SDMMC_STA_TXUNDERR | \
     SDMMC_STA_DTIMEOUT | SDMMC_STA_CTIMEOUT | \
     SDMMC_STA_DCRCFAIL | SDMMC_STA_CCRCFAIL)

#define SDIO_MASKR_ALL \
    (SDMMC_MASK_CCRCFAILIE | SDMMC_MASK_DCRCFAILIE | SDMMC_MASK_CTIMEOUTIE | \
     SDMMC_MASK_TXUNDERRIE | SDMMC_MASK_RXOVERRIE | SDMMC_MASK_CMDRENDIE | \
     SDMMC_MASK_CMDSENTIE | SDMMC_MASK_DATAENDIE | SDMMC_MASK_ACKTIMEOUTIE)

#define HW_SDIO_DATATIMEOUT                 (0xFFFFFFFFU)

extern void sdcard_change(void);

#endif /* __DRV_SDIO_H__ */
