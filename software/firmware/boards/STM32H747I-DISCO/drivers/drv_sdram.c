
#include "board.h"
#include "hw_config.h"
#include "drv_sdram.h" 
#include "is42s32800j.h"

#define DBG_SECTION_NAME  "drv.sdram"
#define DBG_COLOR
#define DBG_LEVEL DBG_LOG
#include <rtdbg.h>

SDRAM_HandleTypeDef SDRAM_Handle;


static void RCC_Configuration(void)
{
  	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(FMC_GPIO_RCC);
	/* FMC clock enable */
	LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_FMC);
  /* MDMA clock enable */
	LL_AHB3_GRP1_EnableClock(LL_AHB3_GRP1_PERIPH_MDMA);
}
static void GPIO_Configuration(void)
{	
	LL_GPIO_InitTypeDef GPIO_InitStruct;
  LL_GPIO_StructInit(&GPIO_InitStruct);
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_12;
	/*地址信号线 针对引脚配置*/
	GPIO_InitStruct.Pin = FMC_A0_PIN; 
	LL_GPIO_Init(FMC_A0_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A1_PIN; 
	LL_GPIO_Init(FMC_A1_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A2_PIN; 
	LL_GPIO_Init(FMC_A2_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A3_PIN; 
	LL_GPIO_Init(FMC_A3_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A4_PIN; 
	LL_GPIO_Init(FMC_A4_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A5_PIN; 
	LL_GPIO_Init(FMC_A5_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A6_PIN; 
	LL_GPIO_Init(FMC_A6_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A7_PIN; 
	LL_GPIO_Init(FMC_A7_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A8_PIN; 
	LL_GPIO_Init(FMC_A8_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A9_PIN; 
	LL_GPIO_Init(FMC_A9_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A10_PIN; 
	LL_GPIO_Init(FMC_A10_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A11_PIN; 
	LL_GPIO_Init(FMC_A11_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_A12_PIN; 
	LL_GPIO_Init(FMC_A12_PORT, &GPIO_InitStruct);

	/*数据信号线 针对引脚配置*/
	GPIO_InitStruct.Pin = FMC_D0_PIN; 
	LL_GPIO_Init(FMC_D0_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D1_PIN; 
	LL_GPIO_Init(FMC_D1_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D2_PIN; 
	LL_GPIO_Init(FMC_D2_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D3_PIN; 
	LL_GPIO_Init(FMC_D3_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D4_PIN; 
	LL_GPIO_Init(FMC_D4_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D5_PIN; 
	LL_GPIO_Init(FMC_D5_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D6_PIN; 
	LL_GPIO_Init(FMC_D6_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D7_PIN; 
	LL_GPIO_Init(FMC_D7_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D8_PIN; 
	LL_GPIO_Init(FMC_D8_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D9_PIN; 
	LL_GPIO_Init(FMC_D9_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D10_PIN; 
	LL_GPIO_Init(FMC_D10_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D11_PIN; 
	LL_GPIO_Init(FMC_D11_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D12_PIN; 
	LL_GPIO_Init(FMC_D12_PORT, &GPIO_InitStruct);;
	GPIO_InitStruct.Pin = FMC_D13_PIN; 
	LL_GPIO_Init(FMC_D13_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D14_PIN; 
	LL_GPIO_Init(FMC_D14_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D15_PIN; 
	LL_GPIO_Init(FMC_D15_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D16_PIN; 
	LL_GPIO_Init(FMC_D16_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D17_PIN; 
	LL_GPIO_Init(FMC_D17_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D18_PIN; 
	LL_GPIO_Init(FMC_D18_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D19_PIN; 
	LL_GPIO_Init(FMC_D19_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D20_PIN; 
	LL_GPIO_Init(FMC_D20_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D21_PIN; 
	LL_GPIO_Init(FMC_D21_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D22_PIN; 
	LL_GPIO_Init(FMC_D22_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D23_PIN; 
	LL_GPIO_Init(FMC_D23_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D24_PIN; 
	LL_GPIO_Init(FMC_D24_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D25_PIN; 
	LL_GPIO_Init(FMC_D25_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D26_PIN; 
	LL_GPIO_Init(FMC_D26_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D27_PIN; 
	LL_GPIO_Init(FMC_D27_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D28_PIN; 
	LL_GPIO_Init(FMC_D28_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D29_PIN; 
	LL_GPIO_Init(FMC_D29_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D30_PIN; 
	LL_GPIO_Init(FMC_D30_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_D31_PIN; 
	LL_GPIO_Init(FMC_D31_PORT, &GPIO_InitStruct);
	/*控制信号线*/
	GPIO_InitStruct.Pin = FMC_CS_PIN; 
	LL_GPIO_Init(FMC_CS_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_BA0_PIN; 
	LL_GPIO_Init(FMC_BA0_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_BA1_PIN; 
	LL_GPIO_Init(FMC_BA1_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_WE_PIN; 
	LL_GPIO_Init(FMC_WE_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_RAS_PIN; 
	LL_GPIO_Init(FMC_RAS_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_CAS_PIN; 
	LL_GPIO_Init(FMC_CAS_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FMC_CLK_PIN; 
	LL_GPIO_Init(FMC_CLK_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FMC_CKE_PIN; 
	LL_GPIO_Init(FMC_CKE_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FMC_DQM0_PIN; 
	LL_GPIO_Init(FMC_DQM0_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_DQM1_PIN; 
	LL_GPIO_Init(FMC_DQM1_PORT, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = FMC_DQM2_PIN; 
	LL_GPIO_Init(FMC_DQM2_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = FMC_DQM3_PIN; 
	LL_GPIO_Init(FMC_DQM3_PORT, &GPIO_InitStruct);
}

static void MDMA_Configuration(SDRAM_HandleTypeDef  *hsdram)
{
  static MDMA_HandleTypeDef mdma_handle;

  /* Configure common MDMA parameters */
  mdma_handle.Init.Request                  = MDMA_REQUEST_SW;
  mdma_handle.Init.TransferTriggerMode      = MDMA_BLOCK_TRANSFER;
  mdma_handle.Init.Priority                 = MDMA_PRIORITY_HIGH;
  mdma_handle.Init.Endianness               = MDMA_LITTLE_ENDIANNESS_PRESERVE;
  mdma_handle.Init.SourceInc                = MDMA_SRC_INC_WORD;
  mdma_handle.Init.DestinationInc           = MDMA_DEST_INC_WORD;
  mdma_handle.Init.SourceDataSize           = MDMA_SRC_DATASIZE_WORD;
  mdma_handle.Init.DestDataSize             = MDMA_DEST_DATASIZE_WORD;
  mdma_handle.Init.DataAlignment            = MDMA_DATAALIGN_PACKENABLE;
  mdma_handle.Init.SourceBurst              = MDMA_SOURCE_BURST_SINGLE;
  mdma_handle.Init.DestBurst                = MDMA_DEST_BURST_SINGLE;
  mdma_handle.Init.BufferTransferLength     = 128;
  mdma_handle.Init.SourceBlockAddressOffset = 0;
  mdma_handle.Init.DestBlockAddressOffset   = 0;
  mdma_handle.Instance                      = SDRAM_MDMAx_CHANNEL;

   /* Associate the MDMA handle */
  __HAL_LINKDMA(hsdram, hmdma, mdma_handle);

  /* Deinitialize the stream for new transfer */
  (void)HAL_MDMA_DeInit(&mdma_handle);

  /* Configure the MDMA stream */
  (void)HAL_MDMA_Init(&mdma_handle);

  /* NVIC configuration for DMA transfer complete interrupt */
  HAL_NVIC_SetPriority(SDRAM_MDMAx_IRQn, 0x0f, 0);
  HAL_NVIC_EnableIRQ(SDRAM_MDMAx_IRQn);
}

 HAL_StatusTypeDef SDRAM_Configuration(SDRAM_HandleTypeDef *hSdram)
{
  FMC_SDRAM_TimingTypeDef sdram_timing;

  /* SDRAM device configuration */
  hSdram->Instance = FMC_SDRAM_DEVICE;

  /* SDRAM handle configuration */
  hSdram->Init.SDBank             = FMC_SDRAM_BANK2;
  hSdram->Init.ColumnBitsNumber   = FMC_SDRAM_COLUMN_BITS_NUM_9;
  hSdram->Init.RowBitsNumber      = FMC_SDRAM_ROW_BITS_NUM_12;
  hSdram->Init.MemoryDataWidth    = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hSdram->Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hSdram->Init.CASLatency         = FMC_SDRAM_CAS_LATENCY_3;
  hSdram->Init.WriteProtection    = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hSdram->Init.SDClockPeriod      = FMC_SDRAM_CLOCK_PERIOD_2;
  hSdram->Init.ReadBurst          = FMC_SDRAM_RBURST_ENABLE;
  hSdram->Init.ReadPipeDelay      = FMC_SDRAM_RPIPE_DELAY_0;

  /* Timing configuration for 100Mhz as SDRAM clock frequency (System clock is up to 200Mhz) */
  sdram_timing.LoadToActiveDelay    = 2;
  sdram_timing.ExitSelfRefreshDelay = 7;
  sdram_timing.SelfRefreshTime      = 4;
  sdram_timing.RowCycleDelay        = 7;
  sdram_timing.WriteRecoveryTime    = 2;
  sdram_timing.RPDelay              = 2;
  sdram_timing.RCDDelay             = 2;

  /* SDRAM controller initialization */
  if(HAL_SDRAM_Init(hSdram, &sdram_timing) != HAL_OK)
  {
    return  HAL_ERROR;
  }
  return HAL_OK;
}


/**
  * @brief  初始化配置使用SDRAM的FMC及GPIO接口，
  *         本函数在SDRAM读写操作前需要被调用
  * @param  None
  * @retval None
  */
int sdram_hw_init(void)
{
  static IS42S32800J_Context_t pRegMode;
	
  RCC_Configuration();
  /* 配置FMC接口相关的 GPIO*/
  GPIO_Configuration();

  MDMA_Configuration(&SDRAM_Handle);
	
	  if(SDRAM_Configuration(&SDRAM_Handle) != HAL_OK)
    {
      LOG_D("sdram init success, mapped at 0x%X, size is %d bytes, data width is %d", SDRAM_DEVICE_ADDR, SDRAM_DEVICE_SIZE, FMC_SDRAM_MEM_BUS_WIDTH_32);
    }
    else
    {
      /* External memory mode register configuration */
      pRegMode.TargetBank      = FMC_SDRAM_CMD_TARGET_BANK2;
      pRegMode.RefreshMode     = IS42S32800J_AUTOREFRESH_MODE_CMD;
      pRegMode.RefreshRate     = REFRESH_COUNT;
      pRegMode.BurstLength     = IS42S32800J_BURST_LENGTH_1;
      pRegMode.BurstType       = IS42S32800J_BURST_TYPE_SEQUENTIAL;
      pRegMode.CASLatency      = IS42S32800J_CAS_LATENCY_3;
      pRegMode.OperationMode   = IS42S32800J_OPERATING_MODE_STANDARD;
      pRegMode.WriteBurstMode  = IS42S32800J_WRITEBURST_MODE_SINGLE;

      /* SDRAM initialization sequence */
      if(IS42S32800J_Init(&SDRAM_Handle, &pRegMode) != IS42S32800J_OK)
      {
        LOG_E("sdram init error\n");
      }
      else
      {
        LOG_D("sdram init success, mapped at 0x%X, size is %d bytes, data width is %d", SDRAM_DEVICE_ADDR, SDRAM_DEVICE_SIZE, FMC_SDRAM_MEM_BUS_WIDTH_32);
      }
    }
		
	

  return 0;
}

INIT_PREV_EXPORT(sdram_hw_init);
