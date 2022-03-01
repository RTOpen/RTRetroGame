/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2020-11-23     xiaoming   first version
 */
#include "board.h"
#include "hw_config.h"

#ifdef RT_USING_SDIO

#if !defined(BSP_USING_SDIO1) && !defined(BSP_USING_SDIO2)
    #error "Please define at least one BSP_USING_SDIOx"
#endif

#include "drv_sdio.h"

//#define DRV_DEBUG
#define DBG_TAG              "drv.sdio"
#ifdef DRV_DEBUG
    #define DBG_LVL               DBG_LOG
#else
    #define DBG_LVL               DBG_INFO
#endif /* DRV_DEBUG */
#include <rtdbg.h>

#define SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS    (1000000)

typedef struct 
{
    struct rt_mmcsd_host *host;
    struct rt_mmcsd_cmd *cmd;
    SDMMC_TypeDef *sdmmc;
    rt_uint8_t *cache_buffer;
    struct rt_event event;
} stm32_sdio_t;

static stm32_sdio_t stm32_sdio1;
static stm32_sdio_t stm32_sdio2;
/**
  * @brief  This function get order from sdio.
  * @param  data
  * @retval sdio order
  */
static int get_order(rt_uint32_t data)
{
    int order = 0;

    switch (data)
    {
    case 1:
        order = 0;
        break;
    case 2:
        order = 1;
        break;
    case 4:
        order = 2;
        break;
    case 8:
        order = 3;
        break;
    case 16:
        order = 4;
        break;
    case 32:
        order = 5;
        break;
    case 64:
        order = 6;
        break;
    case 128:
        order = 7;
        break;
    case 256:
        order = 8;
        break;
    case 512:
        order = 9;
        break;
    case 1024:
        order = 10;
        break;
    case 2048:
        order = 11;
        break;
    case 4096:
        order = 12;
        break;
    case 8192:
        order = 13;
        break;
    case 16384:
        order = 14;
        break;
    default :
        order = 0;
        break;
    }
    return order;
}
static void RCC_Configuration(SDMMC_TypeDef *sdmmc)
{
    if (sdmmc == SDMMC1)
    {
        /* Enable the peripheral clock of SDMMC GPIO */
        LL_AHB4_GRP1_EnableClock(SDMMC1_GPIO_RCC);
        /* Enable SDMMC1 Clock */
        LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_SDMMC1);
    }
    else if (sdmmc == SDMMC2)
    {
        /* Enable the peripheral clock of SDMMC GPIO */
        LL_AHB4_GRP1_EnableClock(SDMMC2_GPIO_RCC);
        /* Enable SDMMC2 Clock */
        LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_SDMMC2);
    }
#ifdef SD_DETECT_GPIO_RCC   
    LL_AHB4_GRP1_EnableClock(SD_DETECT_GPIO_RCC);
#endif
}

static void GPIO_Configuration(SDMMC_TypeDef *sdmmc)
{
    /* GPIO Init Structure */
    LL_GPIO_InitTypeDef  GPIO_InitStruct;

    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Mode      = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Pull      = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed     = LL_GPIO_SPEED_HIGH;

    if (sdmmc == SDMMC1)
    {
        /* SDMMC1 GPIO pin configuration*/
        GPIO_InitStruct.Pin       = SDMMC1_D0_PIN;
        GPIO_InitStruct.Alternate = SDMMC1_D0_AF;
        LL_GPIO_Init(SDMMC1_D0_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC1_D1_PIN;
        GPIO_InitStruct.Alternate = SDMMC1_D1_AF;
        LL_GPIO_Init(SDMMC1_D1_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC1_D2_PIN;
        GPIO_InitStruct.Alternate = SDMMC1_D2_AF;
        LL_GPIO_Init(SDMMC1_D2_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC1_D3_PIN;
        GPIO_InitStruct.Alternate = SDMMC1_D3_AF;
        LL_GPIO_Init(SDMMC1_D3_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC1_CLK_PIN;
        GPIO_InitStruct.Alternate = SDMMC1_CLK_AF;
        LL_GPIO_Init(SDMMC1_CLK_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC1_CMD_PIN;
        GPIO_InitStruct.Alternate = SDMMC1_CMD_AF;
        LL_GPIO_Init(SDMMC1_CMD_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = LL_GPIO_PIN_2;
        GPIO_InitStruct.Mode      = LL_GPIO_MODE_OUTPUT;
        LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
        LL_GPIO_ResetOutputPin(GPIOC, LL_GPIO_PIN_2);
    }
    else if (sdmmc == SDMMC2)
    {
        /* SDMMC GPIO pin configuration*/
        GPIO_InitStruct.Pin       = SDMMC2_D0_PIN;
        GPIO_InitStruct.Alternate = SDMMC2_D0_AF;
        LL_GPIO_Init(SDMMC2_D0_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC2_D1_PIN;
        GPIO_InitStruct.Alternate = SDMMC2_D1_AF;
        LL_GPIO_Init(SDMMC2_D1_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC2_D2_PIN;
        GPIO_InitStruct.Alternate = SDMMC2_D2_AF;
        LL_GPIO_Init(SDMMC2_D2_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC2_D3_PIN;
        GPIO_InitStruct.Alternate = SDMMC2_D3_AF;
        LL_GPIO_Init(SDMMC2_D3_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC2_CLK_PIN;
        GPIO_InitStruct.Alternate = SDMMC2_CLK_AF;
        LL_GPIO_Init(SDMMC2_CLK_PORT, &GPIO_InitStruct);
        GPIO_InitStruct.Pin       = SDMMC2_CMD_PIN;
        GPIO_InitStruct.Alternate = SDMMC2_CMD_AF;
        LL_GPIO_Init(SDMMC2_CMD_PORT, &GPIO_InitStruct);
    }
#ifdef SD_DETECT_PORT             
    GPIO_InitStruct.Pin       = SD_DETECT_PIN;
    GPIO_InitStruct.Mode      = LL_GPIO_MODE_INPUT;
    LL_GPIO_Init(SD_DETECT_PORT, &GPIO_InitStruct);
#endif    
}
static void NVIC_Configuration(SDMMC_TypeDef *sdmmc)
{
    if (sdmmc == SDMMC1)
    {
        HAL_NVIC_SetPriority(SDMMC1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SDMMC1_IRQn);
    }
    else if (sdmmc == SDMMC2)
    {
        HAL_NVIC_SetPriority(SDMMC2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SDMMC2_IRQn);
    }
}
/**
  * @brief  This function wait sdio cmd completed.
  * @param  sdio rthw_sdio
  * @retval None
  */
static void rthw_sdio_wait_completed(stm32_sdio_t *sdio)
{
    rt_uint32_t status;
    struct rt_mmcsd_cmd *cmd = sdio->cmd;
    SDMMC_TypeDef *sdmmc = sdio->sdmmc;

    if (rt_event_recv(&sdio->event, 0xffffffff, RT_EVENT_FLAG_OR | RT_EVENT_FLAG_CLEAR,
                      rt_tick_from_millisecond(5000), &status) != RT_EOK)
    {
        LOG_E("wait cmd completed timeout");
        cmd->err = -RT_ETIMEOUT;
        return;
    }

    cmd->resp[0] = sdmmc->RESP1;
    if (resp_type(cmd) == RESP_R2)
    {
        cmd->resp[1] = sdmmc->RESP2;
        cmd->resp[2] = sdmmc->RESP3;
        cmd->resp[3] = sdmmc->RESP4;
    }

    if (status & SDIO_ERRORS)
    {
        if ((status & SDMMC_STA_CCRCFAIL) && (resp_type(cmd) & (RESP_R3 | RESP_R4)))
        {
            cmd->err = RT_EOK;
        }
        else
        {
            cmd->err = -RT_ERROR;
        }
    }
    else
    {
        cmd->err = RT_EOK;
    }


    if (cmd->err == RT_EOK)
    {
        LOG_D("sta:0x%08X [%08X %08X %08X %08X]", status, cmd->resp[0], cmd->resp[1], cmd->resp[2], cmd->resp[3]);
    }
    else
    {
        LOG_D("send command error = %d", cmd->err);
    }
}

/**
  * @brief  This function send command.
  * @param  sdio rthw_sdio
  * @param  pkg  sdio package
  * @retval None
  */
static void rthw_sdio_send_command(stm32_sdio_t *sdio, struct rt_mmcsd_cmd *cmd)
{
    struct rt_mmcsd_data *data = cmd->data;
    SDMMC_TypeDef *sdmmc = sdio->sdmmc;
    rt_uint32_t reg_cmd;

    rt_event_control(&sdio->event, RT_IPC_CMD_RESET, RT_NULL);
    /* save pkg */
    sdio->cmd = cmd;

    LOG_D("CMD:%d ARG:0x%08x RES:%s%s%s%s%s%s%s%s%s rw:%c len:%d blksize:%d\n",
          cmd->cmd_code,
          cmd->arg,
          resp_type(cmd) == RESP_NONE ? "NONE"  : "",
          resp_type(cmd) == RESP_R1  ? "R1"  : "",
          resp_type(cmd) == RESP_R1B ? "R1B"  : "",
          resp_type(cmd) == RESP_R2  ? "R2"  : "",
          resp_type(cmd) == RESP_R3  ? "R3"  : "",
          resp_type(cmd) == RESP_R4  ? "R4"  : "",
          resp_type(cmd) == RESP_R5  ? "R5"  : "",
          resp_type(cmd) == RESP_R6  ? "R6"  : "",
          resp_type(cmd) == RESP_R7  ? "R7"  : "",
          data ? (data->flags & DATA_DIR_WRITE ?  'w' : 'r') : '-',
          data ? data->blks * data->blksize : 0,
          data ? data->blksize : 0
         );

    sdmmc->MASK |= SDIO_MASKR_ALL;
    reg_cmd = cmd->cmd_code | SDMMC_CMD_CPSMEN;

    /* data pre configuration */
    if (data != RT_NULL)
    {
        SCB_CleanInvalidateDCache();

        reg_cmd |= SDMMC_CMD_CMDTRANS;
        sdmmc->MASK &= ~(SDMMC_MASK_CMDRENDIE | SDMMC_MASK_CMDSENTIE);
        sdmmc->DTIMER = HW_SDIO_DATATIMEOUT;
        sdmmc->DLEN = data->blks * data->blksize;
        sdmmc->DCTRL = (get_order(data->blksize) << 4) | (data->flags & DATA_DIR_READ ? SDMMC_DCTRL_DTDIR : 0);
        sdmmc->IDMABASE0 = (rt_uint32_t)sdio->cache_buffer;
        sdmmc->IDMACTRL = SDMMC_IDMA_IDMAEN;
    }

    if (resp_type(cmd) == RESP_R2)
        reg_cmd |= SDMMC_CMD_WAITRESP;
    else if (resp_type(cmd) != RESP_NONE)
        reg_cmd |= SDMMC_CMD_WAITRESP_0;

    sdmmc->ARG = cmd->arg;
    sdmmc->CMD = reg_cmd;
    /* wait completed */
    rthw_sdio_wait_completed(sdio);

    /* Waiting for data to be sent to completion */
    if (data != RT_NULL)
    {
        volatile rt_uint32_t count = SDIO_TX_RX_COMPLETE_TIMEOUT_LOOPS;

        while (count && (sdmmc->STA & SDMMC_STA_DPSMACT))
        {
            count--;
        }
        if ((count == 0) || (sdmmc->STA & SDIO_ERRORS))
        {
            cmd->err = -RT_ERROR;
        }
    }

    /* data post configuration */
    if (data != RT_NULL)
    {
        if (data->flags & DATA_DIR_READ)
        {
            rt_memcpy(data->buf, sdio->cache_buffer, data->blks * data->blksize);
            SCB_CleanInvalidateDCache();
        }
    }
}

/**
  * @brief  This function send sdio request.
  * @param  sdio  rthw_sdio
  * @param  req   request
  * @retval None
  */
static void rthw_sdio_request(struct rt_mmcsd_host *host, struct rt_mmcsd_req *req)
{
    struct rt_mmcsd_cmd *cmd = RT_NULL;
    stm32_sdio_t *sdio = (stm32_sdio_t*)host->private_data;
    struct rt_mmcsd_data *data;

    if (req->cmd != RT_NULL)
    {
        data = req->cmd->data;
        cmd = req->cmd;

        if (data != RT_NULL)
        {
            rt_uint32_t size = data->blks * data->blksize;

            RT_ASSERT(size <= SDIO_BUFF_SIZE);

            if (data->flags & DATA_DIR_WRITE)
            {
                rt_memcpy(sdio->cache_buffer, data->buf, size);
            }
        }

        rthw_sdio_send_command(sdio, cmd);
    }

    if (req->stop != RT_NULL)
    {
        cmd = req->stop;
        rthw_sdio_send_command(sdio, cmd);
    }

    mmcsd_req_complete(sdio->host);
}


/**
  * @brief  This function interrupt process function.
  * @param  host  rt_mmcsd_host
  * @retval None
  */
void rthw_sdio_irq_process(stm32_sdio_t *sdio)
{
    SDMMC_TypeDef *sdmmc = sdio->sdmmc;
    rt_uint32_t intstatus = sdmmc->STA;

    /* clear irq flag*/
    sdmmc->ICR = intstatus;

    rt_event_send(&sdio->event, intstatus);
}

/**
  * @brief  This function config sdio.
  * @param  host    rt_mmcsd_host
  * @param  io_cfg  rt_mmcsd_io_cfg
  * @retval None
  */
static void rthw_sdio_iocfg(struct rt_mmcsd_host *host, struct rt_mmcsd_io_cfg *io_cfg)
{
    rt_uint32_t temp, clk_src;
    rt_uint32_t clk = io_cfg->clock;
    stm32_sdio_t *sdio = (stm32_sdio_t*)host->private_data;
    SDMMC_TypeDef *sdmmc = sdio->sdmmc;

    LOG_D("clk:%dK width:%s%s%s power:%s%s%s",
          clk / 1000,
          io_cfg->bus_width == MMCSD_BUS_WIDTH_8 ? "8" : "",
          io_cfg->bus_width == MMCSD_BUS_WIDTH_4 ? "4" : "",
          io_cfg->bus_width == MMCSD_BUS_WIDTH_1 ? "1" : "",
          io_cfg->power_mode == MMCSD_POWER_OFF ? "OFF" : "",
          io_cfg->power_mode == MMCSD_POWER_UP ? "UP" : "",
          io_cfg->power_mode == MMCSD_POWER_ON ? "ON" : ""
         );

    clk_src = SDIO_CLOCK_FREQ;

    if (clk > 0)
    {
        if (clk > host->freq_max)
        {
            clk = host->freq_max;
        }
        temp = DIV_ROUND_UP(clk_src, 2 * clk);
        if (temp > 0x3FF)
        {
            temp = 0x3FF;
        }
    }

    if (io_cfg->bus_width == MMCSD_BUS_WIDTH_4)
    {
        temp |= SDMMC_CLKCR_WIDBUS_0;
    }
    else if (io_cfg->bus_width == MMCSD_BUS_WIDTH_8)
    {
        temp |= SDMMC_CLKCR_WIDBUS_1;
    }

    sdmmc->CLKCR = temp;

    if (io_cfg->power_mode == MMCSD_POWER_ON)
        sdmmc->POWER |= SDMMC_POWER_PWRCTRL;
}

static const struct rt_mmcsd_host_ops ops =
{
    rthw_sdio_request,
    rthw_sdio_iocfg,
    RT_NULL,
    RT_NULL,
};

/**
  * @brief  This function create mmcsd host.
  * @param  sdio_des stm32_sdio_des
  * @retval rt_mmcsd_host
  */
rt_err_t sdio_host_init(stm32_sdio_t *sdio)
{
    struct rt_mmcsd_host *host;
    if (sdio == RT_NULL)
    {
        return -RT_ERROR;
    }

    RCC_Configuration(sdio->sdmmc);
    GPIO_Configuration(sdio->sdmmc);
    NVIC_Configuration(sdio->sdmmc);

    host = mmcsd_alloc_host();
    if (host == RT_NULL)
    {
        LOG_E("alloc host fail");
        return -RT_ERROR;
    }
    sdio->cache_buffer = rt_malloc_align(SDIO_BUFF_SIZE, SDIO_ALIGN_LEN);
    if (sdio->cache_buffer == RT_NULL)
    {
        LOG_E("alloc cache_buffer fail");
        mmcsd_free_host(host);
        return -RT_ERROR;
    }
    if (sdio->sdmmc == SDMMC1)
    {
        rt_event_init(&sdio->event, "sdio", RT_IPC_FLAG_FIFO);
    }

    if (sdio->sdmmc == SDMMC2)
    {
        rt_event_init(&sdio->event, "sdio2", RT_IPC_FLAG_FIFO);
    }

    /* set host default attributes */
    host->ops = &ops;
    host->freq_min = 400 * 1000;
    host->freq_max = SDIO_MAX_FREQ;
    host->valid_ocr = VDD_32_33 | VDD_33_34;/* The voltage range supported is 3.2v-3.4v */

    host->flags = MMCSD_BUSWIDTH_4 | MMCSD_MUTBLKWRITE | MMCSD_SUP_HIGHSPEED;

    host->max_seg_size = SDIO_BUFF_SIZE;
    host->max_dma_segs = 1;
    host->max_blk_size = 512;
    host->max_blk_count = 512;

    /* link up host and sdio */
    sdio->host = host;
    host->private_data = sdio;

    /* ready to change */
    mmcsd_change(host);

    return RT_EOK;
}

void SDMMC1_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* Process All SDIO Interrupt Sources */
    rthw_sdio_irq_process(&stm32_sdio1);
    /* leave interrupt */
    rt_interrupt_leave();
}

void SDMMC2_IRQHandler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();
    /* Process All SDIO Interrupt Sources */
    rthw_sdio_irq_process(&stm32_sdio2);
    /* leave interrupt */
    rt_interrupt_leave();
}


int rt_hw_sdio_init(void)
{
    rt_err_t ret = RT_EOK;
#ifdef BSP_USING_SDIO1
    stm32_sdio1.sdmmc = SDMMC1;
    ret = sdio_host_init(&stm32_sdio1);
    if (ret != RT_EOK)
    {
        LOG_E("sdio1 host init fail");
        return -RT_ERROR;
    }
#endif

#ifdef BSP_USING_SDIO2
    //sdmmc2 wifi
    stm32_sdio2.sdmmc = SDMMC2;
    ret = sdio_host_init(&stm32_sdio2);
    if (ret != RT_EOK)
    {
        LOG_E("sdio2 host init fail");
        return -RT_ERROR;
    }
#endif

    return 0;
}
INIT_DEVICE_EXPORT(rt_hw_sdio_init);

void sdcard_change(void)
{
    mmcsd_change(stm32_sdio1.host);
}

#endif /* RT_USING_SDIO */
