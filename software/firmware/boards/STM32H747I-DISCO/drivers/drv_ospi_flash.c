/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-12-04     zylx         first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>

#define DRV_DEBUG
#define LOG_TAG             "drv.ospiflash"
#include <drv_log.h>

#define OSPI_FLASH_SIZE             26
/* Flash commands */
#define OCTAL_IO_READ_CMD           0xEC13
#define OCTAL_PAGE_PROG_CMD         0x12ED
#define OCTAL_READ_STATUS_REG_CMD   0x05FA
#define OCTAL_SECTOR_ERASE_CMD      0x21DE
#define OCTAL_WRITE_ENABLE_CMD      0x06F9
#define READ_STATUS_REG_CMD         0x05
#define WRITE_CFG_REG_2_CMD         0x72
#define WRITE_ENABLE_CMD            0x06
/* Dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ     6
#define DUMMY_CLOCK_CYCLES_READ_REG 4
/* Auto-polling values */
#define WRITE_ENABLE_MATCH_VALUE    0x02
#define WRITE_ENABLE_MASK_VALUE     0x02
#define MEMORY_READY_MATCH_VALUE    0x00
#define MEMORY_READY_MASK_VALUE     0x01
#define AUTO_POLLING_INTERVAL       0x10
/* Memory registers address */
#define CONFIG_REG2_ADDR1           0x0000000
#define CR2_STR_OPI_ENABLE          0x01
#define CONFIG_REG2_ADDR3           0x00000300
#define CR2_DUMMY_CYCLES_66MHZ      0x07
/* Memory delay */
#define MEMORY_REG_WRITE_DELAY      10
#define MEMORY_PAGE_PROG_DELAY      2
/* End address of the OSPI memory */
#define OSPI_END_ADDR               (1 << OSPI_FLASH_SIZE)
/* Size of buffers */
#define BUFFERSIZE                  (COUNTOF(flashTxBuffer) - 1)
/* Exported macro ------------------------------------------------------------*/
#define COUNTOF(__BUFFER__)         (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))

/* Private function prototypes -----------------------------------------------*/
static void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi);
static void OSPI_AutoPollingMemReady(OSPI_HandleTypeDef *hospi);
static void OSPI_OctalModeCfg(OSPI_HandleTypeDef *hospi);

OSPI_HandleTypeDef OSPIHandle;
/* Buffer used for transmission */
static const char *flashTxBuffer = " ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication****  ****Memory-mapped OSPI communication**** ";

int OSPI_FLASH_Init(void)
{
    OSPI_RegularCmdTypeDef sCommand = {0};
    OSPI_MemoryMappedTypeDef sMemMappedCfg = {0};

    /* Initialize OctoSPI ----------------------------------------------------- */
    OSPIHandle.Instance = OCTOSPI2;
    HAL_OSPI_DeInit(&OSPIHandle);

    OSPIHandle.Init.FifoThreshold         = 4;
    OSPIHandle.Init.DualQuad              = HAL_OSPI_DUALQUAD_DISABLE;
    OSPIHandle.Init.MemoryType            = HAL_OSPI_MEMTYPE_MICRON;
    OSPIHandle.Init.DeviceSize            = OSPI_FLASH_SIZE;
    OSPIHandle.Init.ChipSelectHighTime    = 2;
    OSPIHandle.Init.FreeRunningClock      = HAL_OSPI_FREERUNCLK_DISABLE;
    OSPIHandle.Init.ClockMode             = HAL_OSPI_CLOCK_MODE_0;
    OSPIHandle.Init.WrapSize              = HAL_OSPI_WRAP_NOT_SUPPORTED;
    OSPIHandle.Init.ClockPrescaler        = 1; /* OctoSPI clock = 120MHz / ClockPrescaler = 120MHz */
    OSPIHandle.Init.SampleShifting        = HAL_OSPI_SAMPLE_SHIFTING_NONE;
    OSPIHandle.Init.DelayHoldQuarterCycle = HAL_OSPI_DHQC_ENABLE;
    OSPIHandle.Init.ChipSelectBoundary    = 0;

    if (HAL_OSPI_Init(&OSPIHandle) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the memory in octal mode ------------------------------------- */
    OSPI_OctalModeCfg(&OSPIHandle);

    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    /* Enable write operations ------------------------------------------ */
    OSPI_WriteEnable(&OSPIHandle);

    /* Erasing Sequence ------------------------------------------------- */
    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction   = OCTAL_SECTOR_ERASE_CMD;
    sCommand.AddressMode   = HAL_OSPI_ADDRESS_8_LINES;
    sCommand.Address       = 0;
    sCommand.DataMode      = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles   = 0;

    if (HAL_OSPI_Command_IT(&OSPIHandle, &sCommand) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure automatic polling mode to wait for end of erase ------ */
    OSPI_AutoPollingMemReady(&OSPIHandle);

    /* Enable write operations ---------------------------------------- */
    OSPI_WriteEnable(&OSPIHandle);

    /* Memory-mapped mode configuration ------------------------------- */
    sCommand.OperationType = HAL_OSPI_OPTYPE_WRITE_CFG;
    sCommand.Instruction   = OCTAL_PAGE_PROG_CMD;
    sCommand.DataMode      = HAL_OSPI_DATA_8_LINES;
    sCommand.NbData        = 1;

    if (HAL_OSPI_Command(&OSPIHandle, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    sCommand.OperationType = HAL_OSPI_OPTYPE_READ_CFG;
    sCommand.Instruction   = OCTAL_IO_READ_CMD;
    sCommand.DummyCycles   = DUMMY_CLOCK_CYCLES_READ;

    if (HAL_OSPI_Command(&OSPIHandle, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    sMemMappedCfg.TimeOutActivation = HAL_OSPI_TIMEOUT_COUNTER_ENABLE;
    sMemMappedCfg.TimeOutPeriod     = 0x20;

    if (HAL_OSPI_MemoryMapped(&OSPIHandle, &sMemMappedCfg) != HAL_OK)
    {
        Error_Handler();
    }

    return RT_EOK;
}
//INIT_BOARD_EXPORT(OSPI_FLASH_Init);

/**
  * @brief  This function send a Write Enable and wait it is effective.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_WriteEnable(OSPI_HandleTypeDef *hospi)
{
    OSPI_RegularCmdTypeDef  sCommand;
    uint8_t reg[2];

    /* Enable write operations ------------------------------------------ */
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = OCTAL_WRITE_ENABLE_CMD;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    sCommand.Instruction    = OCTAL_READ_STATUS_REG_CMD;
    sCommand.Address        = 0x0;
    sCommand.AddressMode    = HAL_OSPI_ADDRESS_8_LINES;
    sCommand.AddressSize    = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.DataMode       = HAL_OSPI_DATA_8_LINES;
    sCommand.DataDtrMode    = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.NbData         = 1;
    sCommand.DummyCycles    = DUMMY_CLOCK_CYCLES_READ_REG;

    do
    {
        if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            Error_Handler();
        }

        if (HAL_OSPI_Receive(hospi, reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            Error_Handler();
        }
    }
    while ((reg[0] & WRITE_ENABLE_MASK_VALUE) != WRITE_ENABLE_MATCH_VALUE);
}

/**
  * @brief  This function read the SR of the memory and wait the EOP.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_AutoPollingMemReady(OSPI_HandleTypeDef *hospi)
{
    OSPI_RegularCmdTypeDef  sCommand;
    uint8_t reg[2];

    /* Configure automatic polling mode to wait for memory ready ------ */
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = OCTAL_READ_STATUS_REG_CMD;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_8_LINES;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_16_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.Address            = 0x0;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_8_LINES;
    sCommand.AddressSize        = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode     = HAL_OSPI_ADDRESS_DTR_DISABLE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_8_LINES;
    sCommand.DataDtrMode        = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.NbData             = 1;
    sCommand.DummyCycles        = DUMMY_CLOCK_CYCLES_READ_REG;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    do
    {
        if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            Error_Handler();
        }

        if (HAL_OSPI_Receive(hospi, reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
        {
            Error_Handler();
        }
    }
    while ((reg[0] & MEMORY_READY_MASK_VALUE) != MEMORY_READY_MATCH_VALUE);
}

/**
  * @brief  This function configure the memory in Octal mode.
  * @param  hospi: OSPI handle
  * @retval None
  */
static void OSPI_OctalModeCfg(OSPI_HandleTypeDef *hospi)
{
    OSPI_RegularCmdTypeDef  sCommand;
    OSPI_AutoPollingTypeDef sConfig;
    uint8_t reg;

    /* Enable write operations ---------------------------------------- */
    sCommand.OperationType      = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.FlashId            = HAL_OSPI_FLASH_ID_1;
    sCommand.Instruction        = WRITE_ENABLE_CMD;
    sCommand.InstructionMode    = HAL_OSPI_INSTRUCTION_1_LINE;
    sCommand.InstructionSize    = HAL_OSPI_INSTRUCTION_8_BITS;
    sCommand.InstructionDtrMode = HAL_OSPI_INSTRUCTION_DTR_DISABLE;
    sCommand.AddressMode        = HAL_OSPI_ADDRESS_NONE;
    sCommand.AlternateBytesMode = HAL_OSPI_ALTERNATE_BYTES_NONE;
    sCommand.DataMode           = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles        = 0;
    sCommand.DQSMode            = HAL_OSPI_DQS_DISABLE;
    sCommand.SIOOMode           = HAL_OSPI_SIOO_INST_EVERY_CMD;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;
    sCommand.DataDtrMode = HAL_OSPI_DATA_DTR_DISABLE;
    sCommand.NbData      = 1;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Match         = WRITE_ENABLE_MATCH_VALUE;
    sConfig.Mask          = WRITE_ENABLE_MASK_VALUE;
    sConfig.MatchMode     = HAL_OSPI_MATCH_MODE_AND;
    sConfig.Interval      = AUTO_POLLING_INTERVAL;
    sConfig.AutomaticStop = HAL_OSPI_AUTOMATIC_STOP_ENABLE;

    if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Write Configuration register 2 (with new dummy cycles) --------- */
    sCommand.Instruction    = WRITE_CFG_REG_2_CMD;
    sCommand.Address        = CONFIG_REG2_ADDR3;
    sCommand.AddressMode    = HAL_OSPI_ADDRESS_1_LINE;
    sCommand.AddressSize    = HAL_OSPI_ADDRESS_32_BITS;
    sCommand.AddressDtrMode = HAL_OSPI_ADDRESS_DTR_DISABLE;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    reg = CR2_DUMMY_CYCLES_66MHZ;

    if (HAL_OSPI_Transmit(hospi, &reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Wait that the memory is ready ---------------------------------- */
    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_NONE;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Match = MEMORY_READY_MATCH_VALUE;
    sConfig.Mask  = MEMORY_READY_MASK_VALUE;

    if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Enable write operations ---------------------------------------- */
    sCommand.Instruction = WRITE_ENABLE_CMD;
    sCommand.DataMode    = HAL_OSPI_DATA_NONE;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    sCommand.Instruction = READ_STATUS_REG_CMD;
    sCommand.DataMode    = HAL_OSPI_DATA_1_LINE;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    sConfig.Match = WRITE_ENABLE_MATCH_VALUE;
    sConfig.Mask  = WRITE_ENABLE_MASK_VALUE;

    if (HAL_OSPI_AutoPolling(hospi, &sConfig, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Write Configuration register 2 (with octal mode) --------------- */
    sCommand.Instruction = WRITE_CFG_REG_2_CMD;
    sCommand.Address     = CONFIG_REG2_ADDR1;
    sCommand.AddressMode = HAL_OSPI_ADDRESS_1_LINE;

    if (HAL_OSPI_Command(hospi, &sCommand, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    reg = CR2_STR_OPI_ENABLE;

    if (HAL_OSPI_Transmit(hospi, &reg, HAL_OSPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        Error_Handler();
    }

    /* Wait that the configuration is effective and check that memory is ready */
    HAL_Delay(MEMORY_REG_WRITE_DELAY);

    /* Wait that the memory is ready ---------------------------------- */
    OSPI_AutoPollingMemReady(hospi);
}

void OCTOSPI2_IRQHandler(void)
{
  HAL_OSPI_IRQHandler(&OSPIHandle);
}

void ospi_flash_test()
{
    __IO uint8_t step = 0;
    __IO uint8_t *mem_addr;
    uint16_t index;

    mem_addr = (uint8_t *)(OCTOSPI2_BASE);
    for (index = 0; index < BUFFERSIZE; index++)
    {
        *mem_addr = flashTxBuffer[index];
        mem_addr++;
    }

    rt_thread_mdelay(100);

    mem_addr = (uint8_t *)(OCTOSPI2_BASE);
    for (index = 0; index < BUFFERSIZE; index++)
    {
        if (*mem_addr != flashTxBuffer[index])
        {
            LOG_E("read data error");
        }
        mem_addr++;
    }

}
MSH_CMD_EXPORT(ospi_flash_test,ospi_flash_test);

#define __is_print(ch) ((unsigned int)((ch) - ' ') < 127u - ' ')
static void dump_hex(const rt_uint8_t *ptr, rt_size_t buflen)
{
    unsigned char *buf = (unsigned char*)ptr;
    int i, j;

    for (i=0; i<buflen; i+=16)
    {
        rt_kprintf("%08X: ", i);

        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%02X ", buf[i+j]);
            else
                rt_kprintf("   ");
        rt_kprintf(" ");

        for (j=0; j<16; j++)
            if (i+j < buflen)
                rt_kprintf("%c", __is_print(buf[i+j]) ? buf[i+j] : '.');
        rt_kprintf("\n");
    }
}

void ospi_flash_dump()
{
    dump_hex((const rt_uint8_t *)OCTOSPI2_BASE, 64);

    dump_hex((const rt_uint8_t *)0x70001000, 64);
}
MSH_CMD_EXPORT(ospi_flash_dump,ospi_flash_dump);

void flash_erase(uint32_t address)
{

    OSPI_RegularCmdTypeDef sCommand = {0};
    /* Erasing Sequence ------------------------------------------------- */
    sCommand.OperationType = HAL_OSPI_OPTYPE_COMMON_CFG;
    sCommand.Instruction   = OCTAL_SECTOR_ERASE_CMD;
    sCommand.AddressMode   = HAL_OSPI_ADDRESS_8_LINES;
    sCommand.Address       = address;
    sCommand.DataMode      = HAL_OSPI_DATA_NONE;
    sCommand.DummyCycles   = 0;

    if (HAL_OSPI_Command_IT(&OSPIHandle, &sCommand) != HAL_OK)
    {
        Error_Handler();
    }
}

void ospi_flash_erase()
{
    flash_erase(0);
}
MSH_CMD_EXPORT(ospi_flash_erase, ospi_flash_erase);
