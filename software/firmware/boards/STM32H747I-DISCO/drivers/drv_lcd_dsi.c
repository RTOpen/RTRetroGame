#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#ifdef BSP_USING_LCD
#include <string.h>
#include "hw_config.h"
#include "drv_gpio.h"
#include "drv_lcd_dsi.h"
#include "otm8009a.h"

#define DRV_DEBUG
#define LOG_TAG             "drv.lcd"
#include <drv_log.h>

typedef struct
{
  uint32_t X0;
  uint32_t X1;
  uint32_t Y0;
  uint32_t Y1;
  uint32_t PixelFormat;
  uint32_t Address;
}LTDC_LayerConfig_t;


static volatile int m_refresh_flag = 0, m_refreshing = 0;

struct drv_lcd_dsi_device
{
    struct rt_device parent;

    struct rt_device_graphic_info lcd_info;

    struct rt_semaphore lcd_lock;

    rt_uint8_t *front_buf;

    rt_uint8_t *back_buf;
};

#define LCD_LAYER_0_ADDRESS                 0xD0000000U
#define LCD_LAYER_1_ADDRESS                 0xD0200000U

struct drv_lcd_dsi_device _lcd;

DSI_HandleTypeDef   hlcd_dsi;
DMA2D_HandleTypeDef hlcd_dma2d;
LTDC_HandleTypeDef  hlcd_ltdc;

/*******************************************************************************
                       BSP Routines:
                                       LTDC
                                       DMA2D
                                       DSI
*******************************************************************************/
/**
  * @brief  Initialize the BSP LTDC Msp.
  * @param  hltdc  LTDC handle
  * @retval None
  */
static void LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
  if(hltdc->Instance == LTDC)
  {
    /** Enable the LTDC clock */
    __HAL_RCC_LTDC_CLK_ENABLE();


    /** Toggle Sw reset of LTDC IP */
    __HAL_RCC_LTDC_FORCE_RESET();
    __HAL_RCC_LTDC_RELEASE_RESET();
  }
}

/**
  * @brief  De-Initializes the BSP LTDC Msp
  * @param  hltdc  LTDC handle
  * @retval None
  */
static void LTDC_MspDeInit(LTDC_HandleTypeDef *hltdc)
{
  if(hltdc->Instance == LTDC)
  {
    /** Force and let in reset state LTDC */
    __HAL_RCC_LTDC_FORCE_RESET();

    /** Disable the LTDC */
    __HAL_RCC_LTDC_CLK_DISABLE();
  }
}

/**
  * @brief  Initialize the BSP DMA2D Msp.
  * @param  hdma2d  DMA2D handle
  * @retval None
  */
static void DMA2D_MspInit(DMA2D_HandleTypeDef *hdma2d)
{
  if(hdma2d->Instance == DMA2D)
  {
    /** Enable the DMA2D clock */
    __HAL_RCC_DMA2D_CLK_ENABLE();

    /** Toggle Sw reset of DMA2D IP */
    __HAL_RCC_DMA2D_FORCE_RESET();
    __HAL_RCC_DMA2D_RELEASE_RESET();
  }
}

/**
  * @brief  De-Initializes the BSP DMA2D Msp
  * @param  hdma2d  DMA2D handle
  * @retval None
  */
static void DMA2D_MspDeInit(DMA2D_HandleTypeDef *hdma2d)
{
  if(hdma2d->Instance == DMA2D)
  {
    /** Disable IRQ of DMA2D IP */
    HAL_NVIC_DisableIRQ(DMA2D_IRQn);

    /** Force and let in reset state DMA2D */
    __HAL_RCC_DMA2D_FORCE_RESET();

    /** Disable the DMA2D */
    __HAL_RCC_DMA2D_CLK_DISABLE();
  }
}

/**
  * @brief  Initialize the BSP DSI Msp.
  * @param  hdsi  DSI handle
  * @retval None
  */
static void DSI_MspInit(DSI_HandleTypeDef *hdsi)
{
  if(hdsi->Instance == DSI)
  {
    /** Enable DSI Host and wrapper clocks */
    __HAL_RCC_DSI_CLK_ENABLE();

    /** Soft Reset the DSI Host and wrapper */
    __HAL_RCC_DSI_FORCE_RESET();
    __HAL_RCC_DSI_RELEASE_RESET();
  }
}

/**
  * @brief  De-Initializes the BSP DSI Msp
  * @param  hdsi  DSI handle
  * @retval None
  */
static void DSI_MspDeInit(DSI_HandleTypeDef *hdsi)
{
  if(hdsi->Instance == DSI)
  {
    /** Disable IRQ of DSI IP */
    HAL_NVIC_DisableIRQ(DSI_IRQn);

    /** Force and let in reset state the DSI Host + Wrapper IPs */
    __HAL_RCC_DSI_FORCE_RESET();

    /** Disable the DSI Host and Wrapper clocks */
    __HAL_RCC_DSI_CLK_DISABLE();
  }
}

/**
  * @brief  BSP LCD Reset
  *         Hw reset the LCD DSI activating its XRES signal (active low for some time)
  *         and deactivating it later.
  * @param  Instance LCD Instance
  */
void BSP_LCD_Reset(void)
{
  GPIO_InitTypeDef  gpio_init_structure;

  LCD_RESET_GPIO_CLK_ENABLE();

  /* Configure the GPIO Reset pin */
  gpio_init_structure.Pin   = LCD_RESET_PIN;
  gpio_init_structure.Mode  = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Pull  = GPIO_PULLUP;
  gpio_init_structure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LCD_RESET_GPIO_PORT , &gpio_init_structure);

  /* Activate XRES active low */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_PORT , LCD_RESET_PIN, GPIO_PIN_RESET);
  HAL_Delay(20);/* wait 20 ms */
  HAL_GPIO_WritePin(LCD_RESET_GPIO_PORT , LCD_RESET_PIN, GPIO_PIN_SET);/* Deactivate XRES */
  HAL_Delay(10);/* Wait for 10ms after releasing XRES before sending commands */
}

/**
  * @brief  Configure LCD control pins (Back-light, Display Enable and TE)
  * @retval None
  */
static void LCD_InitSequence(void)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* LCD_BL_CTRL GPIO configuration */
  LCD_BL_CTRL_GPIO_CLK_ENABLE();

  gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_OUTPUT_PP;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_structure.Pull      = GPIO_NOPULL;

  HAL_GPIO_Init(LCD_BL_CTRL_GPIO_PORT, &gpio_init_structure);
  /* Assert back-light LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_BL_CTRL_GPIO_PORT, LCD_BL_CTRL_PIN, GPIO_PIN_SET);

  /* LCD_TE_CTRL GPIO configuration */
  LCD_TE_GPIO_CLK_ENABLE();

  gpio_init_structure.Pin       = LCD_TE_PIN;
  gpio_init_structure.Mode      = GPIO_MODE_INPUT;
  gpio_init_structure.Speed     = GPIO_SPEED_FREQ_HIGH;

  HAL_GPIO_Init(LCD_TE_GPIO_PORT, &gpio_init_structure);
  /* Assert back-light LCD_BL_CTRL pin */
  HAL_GPIO_WritePin(LCD_TE_GPIO_PORT, LCD_TE_PIN, GPIO_PIN_SET);

      /** @brief NVIC configuration for LTDC interrupt that is now enabled */
  HAL_NVIC_SetPriority(LTDC_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(LTDC_IRQn);

  /** @brief NVIC configuration for DMA2D interrupt that is now enabled */
  HAL_NVIC_SetPriority(DMA2D_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DMA2D_IRQn);

  /** @brief NVIC configuration for DSI interrupt that is now enabled */
  HAL_NVIC_SetPriority(DSI_IRQn, 0x0F, 0);
  HAL_NVIC_EnableIRQ(DSI_IRQn);
}

/**
  * @brief  DeInitializes LCD GPIO special pins MSP.
  * @retval None
  */
static void LCD_DeInitSequence(void)
{
  GPIO_InitTypeDef  gpio_init_structure;

  /* LCD_BL_CTRL GPIO configuration */
  /* LCD_BL_CTRL GPIO deactivation */
  gpio_init_structure.Pin       = LCD_BL_CTRL_PIN;
  HAL_GPIO_DeInit(LCD_BL_CTRL_GPIO_PORT, gpio_init_structure.Pin);

  /* LCD_TE_CTRL GPIO configuration */
}

/**
  * @brief  Initializes the DSIHOST.
  * @param  hdsi   DSI handle
  * @param  Width  Horizontal active width
  * @param  Height Vertical active height
  * @param  PixelFormat DSI color coding RGB888 or RGB565
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_DSIHOST_DSI_Init(DSI_HandleTypeDef *hdsi, uint32_t Width, uint32_t Height, uint32_t PixelFormat)
{
  DSI_PLLInitTypeDef PLLInit;
  DSI_VidCfgTypeDef VidCfg;

  hdsi->Instance = DSI;
  hdsi->Init.AutomaticClockLaneControl = DSI_AUTO_CLK_LANE_CTRL_DISABLE;
  hdsi->Init.TXEscapeCkdiv = 4;
  hdsi->Init.NumberOfLanes = DSI_TWO_DATA_LANES;
  PLLInit.PLLNDIV = 100;
  PLLInit.PLLIDF = DSI_PLL_IN_DIV5;
  PLLInit.PLLODF = DSI_PLL_OUT_DIV1;
  if (HAL_DSI_Init(hdsi, &PLLInit) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Timing parameters for all Video modes */
  /*
  The lane byte clock is set 62500 Khz
  The pixel clock is set to 27429 Khz
  */
  VidCfg.VirtualChannelID = 0;
  VidCfg.ColorCoding = PixelFormat;
  VidCfg.LooselyPacked = DSI_LOOSELY_PACKED_DISABLE;
  VidCfg.Mode = DSI_VID_MODE_BURST;
  VidCfg.PacketSize = Width;
  VidCfg.NumberOfChunks = 0;
  VidCfg.NullPacketSize = 0xFFFU;
  VidCfg.HSPolarity = DSI_HSYNC_ACTIVE_HIGH;
  VidCfg.VSPolarity = DSI_VSYNC_ACTIVE_HIGH;
  VidCfg.DEPolarity = DSI_DATA_ENABLE_ACTIVE_HIGH;
  VidCfg.HorizontalSyncActive = (OTM8009A_480X800_HSYNC * 62500U)/27429U;
  VidCfg.HorizontalBackPorch = (OTM8009A_480X800_HBP * 62500U)/27429U;
  VidCfg.HorizontalLine = ((Width + OTM8009A_480X800_HSYNC + OTM8009A_480X800_HBP + OTM8009A_480X800_HFP) * 62500U)/27429U;
  VidCfg.VerticalSyncActive = OTM8009A_480X800_VSYNC;
  VidCfg.VerticalBackPorch = OTM8009A_480X800_VBP;
  VidCfg.VerticalFrontPorch = OTM8009A_480X800_VFP;
  VidCfg.VerticalActive = Height;
  VidCfg.LPCommandEnable = DSI_LP_COMMAND_ENABLE;
  VidCfg.LPLargestPacketSize = 4;
  VidCfg.LPVACTLargestPacketSize = 4;

  VidCfg.LPHorizontalFrontPorchEnable  = DSI_LP_HFP_ENABLE;
  VidCfg.LPHorizontalBackPorchEnable   = DSI_LP_HBP_ENABLE;
  VidCfg.LPVerticalActiveEnable        = DSI_LP_VACT_ENABLE;
  VidCfg.LPVerticalFrontPorchEnable    = DSI_LP_VFP_ENABLE;
  VidCfg.LPVerticalBackPorchEnable     = DSI_LP_VBP_ENABLE;
  VidCfg.LPVerticalSyncActiveEnable    = DSI_LP_VSYNC_ENABLE;
  VidCfg.FrameBTAAcknowledgeEnable     = DSI_FBTAA_DISABLE;

  if (HAL_DSI_ConfigVideoMode(hdsi, &VidCfg) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/**
  * @brief  Initializes the LTDC.
  * @param  hltdc  LTDC handle
  * @param  Width  LTDC width
  * @param  Height LTDC height
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_LTDC_Init(LTDC_HandleTypeDef *hltdc, uint32_t Width, uint32_t Height)
{
  hltdc->Instance = LTDC;
  hltdc->Init.HSPolarity = LTDC_HSPOLARITY_AL;
  hltdc->Init.VSPolarity = LTDC_VSPOLARITY_AL;
  hltdc->Init.DEPolarity = LTDC_DEPOLARITY_AL;
  hltdc->Init.PCPolarity = LTDC_PCPOLARITY_IPC;

  hltdc->Init.HorizontalSync     = OTM8009A_480X800_HSYNC - 1;
  hltdc->Init.AccumulatedHBP     = OTM8009A_480X800_HSYNC + OTM8009A_480X800_HBP - 1;
  hltdc->Init.AccumulatedActiveW = OTM8009A_480X800_HSYNC + Width + OTM8009A_480X800_HBP - 1;
  hltdc->Init.TotalWidth         = OTM8009A_480X800_HSYNC + Width + OTM8009A_480X800_HBP + OTM8009A_480X800_HFP - 1;
  hltdc->Init.VerticalSync       = OTM8009A_480X800_VSYNC - 1;
  hltdc->Init.AccumulatedVBP     = OTM8009A_480X800_VSYNC + OTM8009A_480X800_VBP - 1;
  hltdc->Init.AccumulatedActiveH = OTM8009A_480X800_VSYNC + Height + OTM8009A_480X800_VBP - 1;
  hltdc->Init.TotalHeigh         = OTM8009A_480X800_VSYNC + Height + OTM8009A_480X800_VBP + OTM8009A_480X800_VFP - 1;

  hltdc->Init.Backcolor.Blue  = 0x00;
  hltdc->Init.Backcolor.Green = 0x00;
  hltdc->Init.Backcolor.Red   = 0x00;

  return HAL_LTDC_Init(hltdc);
}

/**
  * @brief  MX LTDC layer configuration.
  * @param  hltdc      LTDC handle
  * @param  LayerIndex Layer 0 or 1
  * @param  Config     Layer configuration
  * @retval HAL status
  */
__weak HAL_StatusTypeDef MX_LTDC_ConfigLayer(LTDC_HandleTypeDef *hltdc, uint32_t LayerIndex, LTDC_LayerConfig_t *Config)
{
  LTDC_LayerCfgTypeDef pLayerCfg;

  pLayerCfg.WindowX0 = Config->X0;
  pLayerCfg.WindowX1 = Config->X1;
  pLayerCfg.WindowY0 = Config->Y0;
  pLayerCfg.WindowY1 = Config->Y1;
  pLayerCfg.PixelFormat = Config->PixelFormat;
  pLayerCfg.Alpha = 255;
  pLayerCfg.Alpha0 = 0;
  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_PAxCA;
  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_PAxCA;
  pLayerCfg.FBStartAdress = Config->Address;
  pLayerCfg.ImageWidth = (Config->X1 - Config->X0);
  pLayerCfg.ImageHeight = (Config->Y1 - Config->Y0);
  pLayerCfg.Backcolor.Blue = 0;
  pLayerCfg.Backcolor.Green = 0;
  pLayerCfg.Backcolor.Red = 0;
  return HAL_LTDC_ConfigLayer(hltdc, &pLayerCfg, LayerIndex);
}

/**
  * @brief  LTDC Clock Config for LCD DSI display.
  * @param  hltdc  LTDC Handle
  *         Being __weak it can be overwritten by the application
  * @retval HAL_status
  */
__weak HAL_StatusTypeDef MX_LTDC_ClockConfig(LTDC_HandleTypeDef *hltdc)
{
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  PeriphClkInitStruct.PeriphClockSelection   = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M      = 5U;
  PeriphClkInitStruct.PLL3.PLL3N      = 132U;
  PeriphClkInitStruct.PLL3.PLL3P      = 2U;
  PeriphClkInitStruct.PLL3.PLL3Q      = 2U;
  PeriphClkInitStruct.PLL3.PLL3R      = 24U;
  PeriphClkInitStruct.PLL3.PLL3RGE    = RCC_PLLCFGR_PLL3RGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN  = 0U;
  return HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

/**
  * @brief  LTDC Clock Config for LCD 2 DPI display.
  * @param  hltdc  LTDC Handle
  *         Being __weak it can be overwritten by the application
  * @retval HAL_status
  */
__weak HAL_StatusTypeDef MX_LTDC_ClockConfig2(LTDC_HandleTypeDef *hltdc)
{
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  PeriphClkInitStruct.PeriphClockSelection    = RCC_PERIPHCLK_LTDC;
  PeriphClkInitStruct.PLL3.PLL3M      = 1U;
  PeriphClkInitStruct.PLL3.PLL3N      = 13U;
  PeriphClkInitStruct.PLL3.PLL3P      = 2U;
  PeriphClkInitStruct.PLL3.PLL3Q      = 2U;
  PeriphClkInitStruct.PLL3.PLL3R      = 12U;
  PeriphClkInitStruct.PLL3.PLL3RGE    = RCC_PLLCFGR_PLL3RGE_2;
  PeriphClkInitStruct.PLL3.PLL3VCOSEL = RCC_PLL3VCOWIDE;
  PeriphClkInitStruct.PLL3.PLL3FRACN  = 0U;
  return HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
}

/*******************************************************************************
* Function Name : otm8009a_read_reg
* Description   : Generic Reading function. It must be fullfilled with either
*                 I2C or SPI reading functions
* Input         : Register Address, length of buffer
* Output        : Data Read
*******************************************************************************/
 int32_t otm8009a_read_reg(uint16_t reg, uint8_t *pdata, uint16_t length)
{
    if(HAL_DSI_Read(&hlcd_dsi, 0, pdata, length, DSI_DCS_SHORT_PKT_READ, reg, pdata) != HAL_OK)
  {
    return OTM8009A_ERROR;
  }
  return OTM8009A_OK;
}

/*******************************************************************************
* Function Name : otm8009a_write_reg
* Description   : Generic Writing function. It must be fullfilled with either
*                 I2C or SPI writing function
* Input         : Register Address, Data to be written, length of buffer
* Output        : None
*******************************************************************************/
 int32_t otm8009a_write_reg(uint16_t reg, const uint8_t *pdata, uint16_t length)
{
    if(length <= 1U)
  {
    if(HAL_DSI_ShortWrite(&hlcd_dsi, 0, DSI_DCS_SHORT_PKT_WRITE_P1, reg, (uint32_t)pdata[length]) != HAL_OK)
    {
      return OTM8009A_ERROR;
    }
  }
  else
  {
    if(HAL_DSI_LongWrite(&hlcd_dsi, 0, DSI_DCS_LONG_PKT_WRITE, length, (uint32_t)reg, (uint8_t*)pdata) != HAL_OK)
    {
      return OTM8009A_ERROR;
    }
  }
  return OTM8009A_OK;
}

static rt_err_t drv_lcd_init(struct rt_device *device)
{
    struct drv_lcd_dsi_device *lcd = (struct drv_lcd_dsi_device *)device;
    /* nothing, right now */
    lcd = lcd;
    return RT_EOK;
}

static rt_err_t drv_lcd_control(struct rt_device *device, int cmd, void *args)
{
    struct drv_lcd_dsi_device *lcd = (struct drv_lcd_dsi_device *)device;

    switch (cmd)
    {
    case RTGRAPHIC_CTRL_RECT_UPDATE:
    {
        /* update */
        memcpy((uint32_t *)_lcd.front_buf,(uint32_t *)_lcd.lcd_info.framebuffer, LCD_DSI_BUF_SIZE);
        m_refresh_flag = 1;
    }
    break;

    case RTGRAPHIC_CTRL_GET_INFO:
    {
        struct rt_device_graphic_info *info = (struct rt_device_graphic_info *)args;

        RT_ASSERT(info != RT_NULL);
        info->pixel_format   = lcd->lcd_info.pixel_format;
        info->bits_per_pixel = LCD_BITS_PER_PIXEL;
        info->width          = lcd->lcd_info.width;
        info->height         = lcd->lcd_info.height;
        //info->pitch 		     = lcd->lcd_info.pitch;
        info->framebuffer    = lcd->lcd_info.framebuffer;
    }
    break;
    }

    return RT_EOK;
}


rt_err_t stm32_lcd_init(struct drv_lcd_dsi_device *lcd)
{
  rt_err_t ret = RT_EOK;
  uint32_t ctrl_pixel_format, ltdc_pixel_format, dsi_pixel_format;
  LTDC_LayerConfig_t config;

  {

    ltdc_pixel_format = LTDC_PIXEL_FORMAT_RGB888;
    dsi_pixel_format = DSI_RGB888;
    ctrl_pixel_format = OTM8009A_FORMAT_RGB888;

    /* Toggle Hardware Reset of the LCD using its XRES signal (active low) */
    BSP_LCD_Reset();


    /* Initialize LCD special pins GPIOs */
    LCD_InitSequence();

    /* Initializes peripherals instance value */
    hlcd_ltdc.Instance = LTDC;
    hlcd_dma2d.Instance = DMA2D;
    hlcd_dsi.Instance = DSI;

    /* MSP initialization */

    LTDC_MspInit(&hlcd_ltdc);

    DMA2D_MspInit(&hlcd_dma2d);

    DSI_MspInit(&hlcd_dsi);

    if(MX_DSIHOST_DSI_Init(&hlcd_dsi, OTM8009A_800X480_WIDTH, OTM8009A_800X480_HEIGHT, dsi_pixel_format) != HAL_OK)
    {
      ret = RT_ERROR;
    }
    else if(MX_LTDC_ClockConfig(&hlcd_ltdc) != HAL_OK)
    {
      ret = RT_ERROR;
    }
    else
    {
     if(MX_LTDC_Init(&hlcd_ltdc, OTM8009A_800X480_WIDTH, OTM8009A_800X480_HEIGHT) != HAL_OK)
     {
       ret = RT_ERROR;
     }
    }

    if(ret == RT_EOK)
    {

      /* Configure default LTDC Layer 0. This configuration can be override by calling
      BSP_LCD_ConfigLayer() at application level */
      config.X0          = 0;
      config.X1          = OTM8009A_800X480_WIDTH;
      config.Y0          = 0;
      config.Y1          = OTM8009A_800X480_HEIGHT;
      config.PixelFormat = ltdc_pixel_format;
      config.Address     = LCD_LAYER_0_ADDRESS;
      if(MX_LTDC_ConfigLayer(&hlcd_ltdc, 0, &config) != HAL_OK)
      {
        ret = RT_ERROR;
      }
      else
      {
				uint32_t id = 0;
        /* Enable the DSI host and wrapper after the LTDC initialization
        To avoid any synchronization issue, the DSI shall be started after enabling the LTDC */
        (void)HAL_DSI_Start(&hlcd_dsi);

        /* Enable the DSI BTW for read operations */
        (void)HAL_DSI_ConfigFlowControl(&hlcd_dsi, DSI_FLOW_CONTROL_BTA);

        /* Initialize the OTM8009A LCD Display IC Driver (KoD LCD IC Driver)
        depending on configuration of DSI */
    if(OTM8009A_ReadID(&id) != OTM8009A_OK)
    {
      ret = RT_ERROR;
    }

    else
    {
      if(OTM8009A_Init(0, OTM8009A_ORIENTATION_LANDSCAPE) != OTM8009A_OK)
      {
        ret = RT_ERROR;
      }
    }

      }
    /* By default the reload is activated and executed immediately */
    //Lcd_Ctx[Instance].ReloadEnable = 1U;
   }
  }

    return RT_EOK;
}


void DSI_IRQHandler(void)
{
    HAL_DSI_IRQHandler(&hlcd_dsi);
}


void HAL_DSI_TearingEffectCallback(DSI_HandleTypeDef *hdsi)
{
    if (m_refresh_flag && !m_refreshing)
    {
        m_refreshing = 1;
        HAL_DSI_Refresh(&hlcd_dsi);
    }
    else if (!m_refreshing)
    {
        //rtgui_send_vsync();
    }
}

void HAL_DSI_EndOfRefreshCallback(DSI_HandleTypeDef *hdsi)
{
    m_refresh_flag = 0;
    m_refreshing = 0;
}

#ifdef RT_USING_DEVICE_OPS
const static struct rt_device_ops lcd_ops =
{
    drv_lcd_init,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    RT_NULL,
    drv_lcd_control
};
#endif

int drv_lcd_hw_init(void)
{
    rt_err_t result = RT_EOK;
    struct rt_device *device = &_lcd.parent;

    /* init lcd_lock semaphore */
    result = rt_sem_init(&_lcd.lcd_lock, "lcd_lock", 0, RT_IPC_FLAG_FIFO);
    if (result != RT_EOK)
    {
        LOG_E("init semaphore failed!\n");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* config LCD dev info */
    _lcd.lcd_info.height = LCD_PIXEL_HEIGHT;
    _lcd.lcd_info.width = LCD_PIXEL_WIDTH;
    _lcd.lcd_info.bits_per_pixel = LCD_BITS_PER_PIXEL;
    _lcd.lcd_info.pixel_format = RTGRAPHIC_PIXEL_FORMAT_RGB888;
    //_lcd.lcd_info.pitch = 1024 * LCD_BITS_PER_PIXEL / 8;

    /* malloc memory */
    _lcd.front_buf = (void *)LCD_LAYER_0_ADDRESS; /* the front round buffer in on chip RAM */
    _lcd.lcd_info.framebuffer = (void *)LCD_LAYER_1_ADDRESS; //rt_malloc_align(LCD_DSI_BUF_SIZE, 32); /* the frame buffer address */

    rt_kprintf("framebuffer addr: %p\n", _lcd.lcd_info.framebuffer);
    rt_kprintf("front_buf addr: %p\n", _lcd.front_buf);

    if (_lcd.lcd_info.framebuffer == RT_NULL || _lcd.front_buf == RT_NULL)
    {
        LOG_E("init frame buffer failed!\n");
        result = -RT_ENOMEM;
        goto __exit;
    }

    /* memset buff to 0xFF */
    memset(_lcd.lcd_info.framebuffer, 0xff, LCD_DSI_BUF_SIZE);
    memset(_lcd.front_buf, 0xff, LCD_DSI_BUF_SIZE);
		
    device->type    = RT_Device_Class_Graphic;
#ifdef RT_USING_DEVICE_OPS
    device->ops     = &lcd_ops;
#else
    device->init    = drv_lcd_init;
    device->control = drv_lcd_control;
#endif

    /* register lcd device */
    rt_device_register(device, "lcd", RT_DEVICE_FLAG_RDWR);

    /* init stm32 LTDC */
    if (stm32_lcd_init(&_lcd) != RT_EOK)
    {
        result = -RT_ERROR;
        goto __exit;
    }


__exit:
    if (result != RT_EOK)
    {
        rt_sem_delete(&_lcd.lcd_lock);

        if (_lcd.lcd_info.framebuffer)
        {
            rt_free(_lcd.lcd_info.framebuffer);
        }

        if (_lcd.front_buf)
        {

            rt_free(_lcd.front_buf);
        }
    }
    return result;
}
INIT_DEVICE_EXPORT(drv_lcd_hw_init);


#ifdef DRV_DEBUG
#ifdef FINSH_USING_MSH
void lcd_dsi_test()
{
    struct drv_lcd_dsi_device *lcd;
    lcd = (struct drv_lcd_dsi_device *)rt_device_find("lcd");
    rt_uint8_t *ptr = lcd->lcd_info.framebuffer;
    for (int i = 0; i < 3; i++)
    {
        /* red */
        for (unsigned long long i = 0; i < LCD_DSI_BUF_SIZE / 3; i++)
        {
            ptr[3 * i + 0] = 0xFF;
            ptr[3 * i + 1] = 0x00;
            ptr[3 * i + 2] = 0x00;
        }
        rt_device_control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);

        /* green */
        for (int i = 0; i < LCD_DSI_BUF_SIZE / 3; i++)
        {
            ptr[3 * i + 0] = 0x00;
            ptr[3 * i + 1] = 0xFF;
            ptr[3 * i + 2] = 0x00;
        }
        rt_device_control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);

        /* blue */
        for (int i = 0; i < LCD_DSI_BUF_SIZE / 3; i++)
        {
            ptr[3 * i + 0] = 0x00;
            ptr[3 * i + 1] = 0x00;
            ptr[3 * i + 2] = 0xFF;
        }
        rt_device_control(&lcd->parent, RTGRAPHIC_CTRL_RECT_UPDATE, RT_NULL);
        rt_thread_mdelay(1000);
    }
}
MSH_CMD_EXPORT(lcd_dsi_test, lcd_dsi_test);

#endif /* FINSH_USING_MSH */
#endif /* DRV_DEBUG */
#endif /* BSP_USING_LCD */
