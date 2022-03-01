#include "drv_spi.h" 
#include "board.h" 
#include "rtdevice.h"

//#define DBG_ENABLE 
#define DBG_SECTION_NAME "drv.spi"
#define DBG_LEVEL        DBG_LOG
#define DBG_COLOR
#include <rtdbg.h> 

typedef struct 
{
    SPI_TypeDef *Instance;
    struct rt_spi_configuration *cfg;
}stm32_spi_t;

struct stm32_spi_cs
{
    rt_uint32_t pin;
};

#if defined(BSP_SPI_ENABLE_PORT3)
static stm32_spi_t spi3 = {.Instance = SPI3}; 
static struct rt_spi_bus spi3_bus = {.parent.user_data = &spi3}; 
#endif

#if defined(BSP_SPI_ENABLE_PORT4)
static stm32_spi_t spi4 = {.Instance = SPI4}; 
static struct rt_spi_bus spi4_bus = {.parent.user_data = &spi4}; 
#endif

static rt_uint32_t get_spi_clk_source_freq(stm32_spi_t *spi_dev)
{
    rt_uint32_t freq = 0;

#if defined(BSP_SPI_ENABLE_PORT3)
    if(spi_dev->Instance == SPI3)
    {
        PLL1_ClocksTypeDef pll1 = {0}; 
        
        HAL_RCCEx_GetPLL1ClockFreq(&pll1); 
        freq = (rt_uint32_t)pll1.PLL1_Q_Frequency; 
    }
#endif

#if defined(BSP_SPI_ENABLE_PORT4)
    if(spi_dev->Instance == SPI4)
    {
        PLL2_ClocksTypeDef pll2 = {0}; 
        
        HAL_RCCEx_GetPLL2ClockFreq(&pll2); 
        freq = (rt_uint32_t)pll2.PLL2_Q_Frequency; 
    }
#endif 

    return freq; 
}

static void RCC_Configuration(stm32_spi_t *spi_dev)
{
  if(spi_dev->Instance==SPI3)
	{
	/* Enable the peripheral clock of SPI GPIO */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOC);
  /* Enable SPI3 Clock */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_SPI3);
	}
	else if(spi_dev->Instance==SPI4)
  {
  /* Enable the peripheral clock of SPI GPIO */
  LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE);
  /* Enable SPI4 Clock */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SPI4);
	}
}

static void GPIO_Configuration(stm32_spi_t *spi_dev)
{
	/* GPIO Init Structure */
  LL_GPIO_InitTypeDef  GPIO_InitStruct;
	
	LL_GPIO_StructInit(&GPIO_InitStruct);
	GPIO_InitStruct.Mode      = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Pull      = LL_GPIO_PULL_DOWN;
  GPIO_InitStruct.Speed     = LL_GPIO_SPEED_HIGH;
	
  if(spi_dev->Instance==SPI3)
	{
  /* SPI3 GPIO pin configuration*/
  GPIO_InitStruct.Pin       = LL_GPIO_PIN_5;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  GPIO_InitStruct.Pin       = LL_GPIO_PIN_10 | LL_GPIO_PIN_11;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_6;
  LL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	}
	else if(spi_dev->Instance==SPI4)
  {
  /* SPI4 GPIO pin configuration*/
  GPIO_InitStruct.Pin       = LL_GPIO_PIN_5 | LL_GPIO_PIN_6 | LL_GPIO_PIN_12;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_5;
  LL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	}
}
static rt_err_t SPI_Configuration(stm32_spi_t *spi_dev)
{
	  rt_uint32_t freq = 0; 
    rt_uint32_t divx = 0;
    rt_uint32_t max_hz = 0; 
	  /* SPI Init Structure */
    LL_SPI_InitTypeDef   SPI_InitStruct;
	  
	  RT_ASSERT(spi_dev != RT_NULL); 
	  
	  LL_SPI_StructInit(&SPI_InitStruct);
	
 
    if((spi_dev->cfg->data_width >= 4) && (spi_dev->cfg->data_width <= 32))
    {
			  LOG_D("SPI_DATASIZE_%dBIT",spi_dev->cfg->data_width); 
			  switch(spi_dev->cfg->data_width)
				{
					case 4:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_4BIT;
						break;
        	case 5:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_5BIT;
						break;
					case 6:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_6BIT;
						break;
					case 7:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_7BIT;
						break;
        	case 8:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
						break;
					case 9:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_9BIT;
						break;
					case 10:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_10BIT;
						break;
        	case 11:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_11BIT;
						break;
					case 12:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_12BIT;
						break;
					case 13:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_13BIT;
						break;
        	case 14:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_14BIT;
						break;
					case 15:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_15BIT;
						break;
					case 16:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_16BIT;
						break;
					case 24:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_24BIT;
						break;
					case 32:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_32BIT;
						break;
					default:
						SPI_InitStruct.DataWidth = LL_SPI_DATAWIDTH_8BIT;
						break;
				}
    }
    else
    {
        LOG_E("SPI data width error."); 
        return (-RT_EINVAL);    
    }

    /* 计算能配置的最接近(不高于)于目标时钟频率的分频数 */ 
    max_hz = spi_dev->cfg->max_hz;  
    freq = get_spi_clk_source_freq(spi_dev);
    divx = divx; 

    if(max_hz >= freq/4)
    {
        divx = 4; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV4;
    }
    else if(max_hz >= freq/8)
    {
        divx = 8; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV8;
    }
    else if(max_hz >= freq/16)
    {
        divx = 16; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV16;
    }
    else if(max_hz >= freq/32)
    {
        divx = 32; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV32;
    }
    else if(max_hz >= freq/64)
    {
        divx = 64; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV64;
    }
    else if(max_hz >= freq/128)
    {
        divx = 128; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV128;
    }
    else
    {
        divx = 256; 
        SPI_InitStruct.BaudRate = LL_SPI_BAUDRATEPRESCALER_DIV256;
    }

    LOG_D("spi clk source freq is %dHz.", freq); 
    LOG_D("spi clk target freq is %dHz.", spi_dev->cfg->max_hz); 
    LOG_D("spi clk divx is %d.", divx); 
    LOG_D("spi clk freq is %dHz.", freq / divx);

    /* CPOL */
    if(spi_dev->cfg->mode & RT_SPI_CPOL)
    {
     LOG_D("SPI_POLARITY_HIGH");     
     SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_HIGH;
    }
    else
    {
     LOG_D("SPI_POLARITY_LOW"); 
     SPI_InitStruct.ClockPolarity = LL_SPI_POLARITY_LOW;
    } 

    /* CPHA */
    if(spi_dev->cfg->mode & RT_SPI_CPHA)
    {
    LOG_D("SPI_PHASE_2EDGE"); 
		SPI_InitStruct.ClockPhase = LL_SPI_PHASE_2EDGE;
    }
    else
    {
    LOG_D("SPI_PHASE_1EDGE"); 
		SPI_InitStruct.ClockPhase = LL_SPI_PHASE_1EDGE;
    }

    /* MSB or LSB */
    if(spi_dev->cfg->mode & RT_SPI_MSB)
    {
        LOG_D("SPI_FIRSTBIT_MSB"); 
        SPI_InitStruct.BitOrder  = LL_SPI_MSB_FIRST;
    }
    else
    {
        LOG_D("SPI_FIRSTBIT_LSB"); 
        SPI_InitStruct.BitOrder  = LL_SPI_LSB_FIRST;
    }

    SPI_InitStruct.TransferDirection = LL_SPI_FULL_DUPLEX;
		SPI_InitStruct.NSS               = LL_SPI_NSS_SOFT;
    SPI_InitStruct.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
    SPI_InitStruct.Mode              = LL_SPI_MODE_MASTER;
		
	  LL_SPI_Init(spi_dev->Instance, &SPI_InitStruct);

		LL_SPI_SetFIFOThreshold(spi_dev->Instance,LL_SPI_FIFO_TH_08DATA);
  
	LL_SPI_DisableIOSwap(spi_dev->Instance);
		
  LL_SPI_DisableGPIOControl(spi_dev->Instance);
		
  LL_SPI_EnableMasterRxAutoSuspend(spi_dev->Instance);

  /* Enable SPI */
  LL_SPI_Enable(spi_dev->Instance);
		
	// 当跑在内部 Flash 时, 速度太快, 导致还没初始化完毕就通讯
  rt_thread_mdelay(5); 
		
  return RT_EOK;
}


static rt_err_t configure(struct rt_spi_device *device, struct rt_spi_configuration *cfg)
{
    rt_err_t ret = RT_EOK; 

    stm32_spi_t *spi_dev  = (stm32_spi_t *)device->bus->parent.user_data;
    spi_dev->cfg = cfg;
	  ret = SPI_Configuration(spi_dev);


    return ret; 
}

static rt_err_t SPI_TransmitReceive(stm32_spi_t *spi_dev, uint8_t *tx_data, uint8_t *rx_data, uint16_t size,
                                          uint32_t timeout)
{
  uint32_t   tickstart;
	uint16_t tx_size = size;
	uint16_t rx_size = size;
  /* Init tickstart for timeout management*/
  tickstart = rt_tick_get();


  if (((tx_data == RT_NULL)&&(rx_data == RT_NULL))||(size==0))
  {
    return RT_ERROR;
  }

  /* Set the number of data at current transfer */
  MODIFY_REG(spi_dev->Instance->CR2, SPI_CR2_TSIZE, size);

  /* Enable SPI peripheral */
  LL_SPI_Enable(spi_dev->Instance);

  if ((spi_dev->cfg->mode & RT_SPI_SLAVE) != RT_SPI_SLAVE)
  {
    /* Master transfer start */
    SET_BIT(spi_dev->Instance->CR1, SPI_CR1_CSTART);
  }

  /* Transmit and Receive data in 32 Bit mode */
  if (spi_dev->cfg->data_width > 16)
  {
    while ((tx_size > 0UL)||(rx_size > 0UL))
    {
      /* Check TXP flag */
      if ((LL_SPI_IsActiveFlag_TXP(spi_dev->Instance)) && (tx_size > 0UL))
      {
				if(tx_data != RT_NULL)
				{
        *((__IO uint32_t *)&spi_dev->Instance->TXDR) = *((uint32_t *)tx_data);
        tx_data += sizeof(uint32_t);
				}else
				{
				*((__IO uint32_t *)&spi_dev->Instance->TXDR) = 0UL;
				}
        tx_size --;
      }

      /* Check RXWNE/EOT flag */
      if (((spi_dev->Instance->SR & (SPI_SR_RXWNE | SPI_SR_EOT)) != 0UL) && (rx_size > 0UL))
      {
				if(rx_data != RT_NULL)
				{
        *((uint32_t *)rx_data) = *((__IO uint32_t *)&spi_dev->Instance->RXDR);
        rx_data += sizeof(uint32_t);
				}else
				{
				 uint32_t temp = *((__IO uint32_t *)&spi_dev->Instance->RXDR);
				}
        rx_size --;
      }
    }

    /* Timeout management */
    if ((((rt_tick_get() - tickstart) >=  timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
    {
			/* Clean flag */
			SET_BIT(spi_dev->Instance->IFCR, SPI_IFCR_EOTC|SPI_IFCR_TXTFC);
      /* Disable SPI peripheral */
      LL_SPI_Disable(spi_dev->Instance);
			
      return RT_ERROR;
    }
  }
  /* Transmit and Receive data in 16 Bit mode */
  else if (spi_dev->cfg->data_width > 8)
  {
    while ((tx_size > 0UL)||(rx_size > 0UL))
    {
      /* Check TXP flag */
      if (LL_SPI_IsActiveFlag_TXP(spi_dev->Instance) && (tx_size > 0UL))
      {
        if ((tx_size > 1UL) && (LL_SPI_GetFIFOThreshold(spi_dev->Instance) > LL_SPI_FIFO_TH_01DATA))
        {
					if(tx_data != RT_NULL)
				  {
          *((__IO uint32_t *)&spi_dev->Instance->TXDR) = *((uint32_t *)tx_data);
          tx_data += sizeof(uint32_t);
					}else
					{
					*((__IO uint32_t *)&spi_dev->Instance->TXDR) = 0UL;
					}
          tx_size -= (uint16_t)2UL;
        }
        else
        {
					if(tx_data != RT_NULL)
				  {
          *((__IO uint16_t *)&spi_dev->Instance->TXDR) = *((uint16_t *)tx_data);
					tx_data += sizeof(uint16_t);
					}else
					{
					*((__IO uint16_t *)&spi_dev->Instance->TXDR) = 0UL;
					}
          tx_size--;
        }
      }

      /* Check RXWNE/FRLVL flag */
      if (((spi_dev->Instance->SR & (SPI_SR_RXWNE | SPI_SR_RXPLVL)) != 0UL) && (rx_size > 0UL))
      {
        if ((spi_dev->Instance->SR & SPI_SR_RXWNE) != 0UL)
        {
					if(rx_data != RT_NULL)
					{
          *((uint32_t *)rx_data) = *((__IO uint32_t *)&spi_dev->Instance->RXDR);
          rx_data += sizeof(uint32_t);
					}else
					{
					uint32_t temp = *((__IO uint32_t *)&spi_dev->Instance->RXDR);
					}
          rx_size -= (uint16_t)2UL;
        }
        else
        {
					if(rx_data != RT_NULL)
					{
          *((uint16_t *)rx_data) = *((__IO uint16_t *)&spi_dev->Instance->RXDR);
          rx_data += sizeof(uint16_t);
					}else
					{
					uint16_t temp = *((__IO uint16_t *)&spi_dev->Instance->RXDR);
					}
          rx_size--;
        }
      }

      /* Timeout management */
      if ((((rt_tick_get() - tickstart) >=  timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
      {
					/* Clean flag */
					SET_BIT(spi_dev->Instance->IFCR, SPI_IFCR_EOTC|SPI_IFCR_TXTFC);
          /* Disable SPI peripheral */
          LL_SPI_Disable(spi_dev->Instance);
				
        return RT_ERROR;
      }
    }
  }
  /* Transmit and Receive data in 8 Bit mode */
  else
  {
    while ((tx_size > 0UL)||(rx_size > 0UL))
    {
      /* check TXP flag */
      if ((LL_SPI_IsActiveFlag_TXP(spi_dev->Instance)) && (tx_size > 0UL))
      {
        if ((tx_size > 3UL) && (LL_SPI_GetFIFOThreshold(spi_dev->Instance) > LL_SPI_FIFO_TH_03DATA))
        {
					if(tx_data != RT_NULL)
				  {
          *((__IO uint32_t *)&spi_dev->Instance->TXDR) = *((uint32_t *)tx_data);
          tx_data += sizeof(uint32_t);
					}else
					{
					*((__IO uint32_t *)&spi_dev->Instance->TXDR) = 0UL;
					}
          tx_size -= (uint16_t)4UL;
        }
        else if ((tx_size > 1UL) && (LL_SPI_GetFIFOThreshold(spi_dev->Instance) > LL_SPI_FIFO_TH_01DATA))
        {
					if(tx_data != RT_NULL)
				  {
          *((__IO uint16_t *)&spi_dev->Instance->TXDR) = *((uint16_t *)tx_data);
          tx_data += sizeof(uint16_t);
					}else
					{
					*((__IO uint16_t *)&spi_dev->Instance->TXDR) = 0UL;
					}
          tx_size -= (uint16_t)2UL;
        }
        else
        {
					if(tx_data != RT_NULL)
				  {
          *((__IO uint8_t *)&spi_dev->Instance->TXDR) = *((uint8_t *)tx_data);
          tx_data += sizeof(uint8_t);
					}else
					{
					*((__IO uint8_t *)&spi_dev->Instance->TXDR) = 0UL;
					}
          tx_size--;
        }
      }

      /* Wait until RXWNE/FRLVL flag is reset */
      if (((spi_dev->Instance->SR & (SPI_SR_RXWNE | SPI_SR_RXPLVL)) != 0UL) && (rx_size > 0UL))
      {
        if ((spi_dev->Instance->SR & SPI_SR_RXWNE) != 0UL)
        {
					if(rx_data != RT_NULL)
					{
          *((uint32_t *)rx_data) = *((__IO uint32_t *)&spi_dev->Instance->RXDR);
          rx_data += sizeof(uint32_t);
					}else
					{
					uint32_t temp = *((__IO uint32_t *)&spi_dev->Instance->RXDR);
					}
          rx_size -= (uint16_t)4UL;
        }
        else if ((spi_dev->Instance->SR & SPI_SR_RXPLVL) > SPI_RX_FIFO_1PACKET)
        {
					if(rx_data != RT_NULL)
					{
          *((uint16_t *)rx_data) = *((__IO uint16_t *)&spi_dev->Instance->RXDR);
          rx_data += sizeof(uint16_t);
					}else
					{
					uint16_t temp = *((__IO uint16_t *)&spi_dev->Instance->RXDR);
					}
          rx_size -= (uint16_t)2UL;
        }
        else
        {
					if(rx_data != RT_NULL)
					{
          *((uint8_t *)rx_data) = *((__IO uint8_t *)&spi_dev->Instance->RXDR);
          rx_data += sizeof(uint8_t);
					}else
					{
					 uint8_t temp = *((__IO uint8_t *)&spi_dev->Instance->RXDR);
					}
          rx_size--;
        }
      }

      /* Timeout management */
      if ((((rt_tick_get() - tickstart) >=  timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
      {
					/* Clean flag */
					SET_BIT(spi_dev->Instance->IFCR, SPI_IFCR_EOTC|SPI_IFCR_TXTFC);
          /* Disable SPI peripheral */
          LL_SPI_Disable(spi_dev->Instance);
				
        return RT_ERROR;
      }
    }
  }

  /* Wait for Tx/Rx (and CRC) data to be sent/received */
  /* Wait until flag is set */
  while (LL_SPI_IsActiveFlag_EOT(spi_dev->Instance) == RESET)
  {
    /* Check for the Timeout */
    if ((((rt_tick_get() - tickstart) >=  timeout) && (timeout != HAL_MAX_DELAY)) || (timeout == 0U))
    {
      return RT_ETIMEOUT;
    }
  }

	/* Clean flag */
	SET_BIT(spi_dev->Instance->IFCR, SPI_IFCR_EOTC|SPI_IFCR_TXTFC);
  /* Disable SPI peripheral */
  LL_SPI_Disable(spi_dev->Instance);

  return RT_EOK;
}

static rt_uint32_t spixfer(struct rt_spi_device *device, struct rt_spi_message *message)
{
    rt_err_t ret = RT_EOK; 
    rt_uint32_t length = message->length; 
    
    RT_ASSERT(device != RT_NULL);
    RT_ASSERT(device->bus != RT_NULL);
    RT_ASSERT(device->bus->parent.user_data != RT_NULL);

    stm32_spi_t *spi_dev = (stm32_spi_t *)device->bus->parent.user_data;
    struct stm32_spi_cs *cs = device->parent.user_data;

    if (message->cs_take)
    {
        rt_pin_write(cs->pin, PIN_LOW);
    }
    
    if(message->length == 0)
    {
        length = 0; 
        goto _ret; 
    }

    ret = SPI_TransmitReceive(spi_dev, (uint8_t *)(message->send_buf),(uint8_t *)(message->recv_buf), message->length, 1000); 
    if(ret != RT_EOK)
      {
            LOG_D("SPI_TransmitReceive = failed.", ret); 
            while(1); 
      }
    
_ret:
    if (message->cs_release)
    {
        rt_pin_write(cs->pin, PIN_HIGH);
    }

    return length;
}

rt_err_t stm32_spi_bus_attach_device(const char *bus_name, const char *device_name, rt_uint32_t pin)
{
    rt_err_t ret = RT_EOK;
    
    struct rt_spi_device *spi_device = (struct rt_spi_device *)rt_malloc(sizeof(struct rt_spi_device)); 
    RT_ASSERT(spi_device != RT_NULL); 

    struct stm32_spi_cs *cs_pin = (struct stm32_spi_cs *)rt_malloc(sizeof(struct stm32_spi_cs));
    RT_ASSERT(cs_pin != RT_NULL);

    cs_pin->pin = pin;
    rt_pin_mode(pin, PIN_MODE_OUTPUT);
    rt_pin_write(pin, PIN_HIGH); 
 
    ret = rt_spi_bus_attach_device(spi_device, device_name, bus_name, (void *)cs_pin); 
    
    return ret; 
}

static struct rt_spi_ops ops = {configure, spixfer}; 

int rt_hw_spi_init(void)
{
#if defined(BSP_SPI_ENABLE_PORT3) 
	  RCC_Configuration(&spi3);
	  GPIO_Configuration(&spi3);
    rt_spi_bus_register(&spi3_bus, "spi3", &ops); 
#endif

#if defined(BSP_SPI_ENABLE_PORT4)
		RCC_Configuration(&spi4);
	  GPIO_Configuration(&spi4);
    rt_spi_bus_register(&spi4_bus, "spi4", &ops); 
#endif

    return RT_EOK; 
}
INIT_DEVICE_EXPORT(rt_hw_spi_init);

int rt_hw_spi_device(void)
{
  stm32_spi_bus_attach_device("spi4","spi40",2);
	return RT_EOK;
}
INIT_COMPONENT_EXPORT(rt_hw_spi_device);
