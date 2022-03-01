#ifndef __DRV_SDRAM_H
#define	__DRV_SDRAM_H

#include "stm32h7xx.h"
#include <stdio.h>



/** @defgroup STM32H747I_DISCO_SDRAM_Exported_Constants Exported Constants
  * @{
  */
#define SDRAM_INSTANCES_NBR       1U
#define SDRAM_DEVICE_ADDR         0xD0000000U
#define SDRAM_DEVICE_SIZE         0x2000000U

/* MDMA definitions for SDRAM DMA transfer */
#define SDRAM_MDMAx_CHANNEL                MDMA_Channel0
#define SDRAM_MDMAx_IRQn                   MDMA_IRQn
#define SDRAM_MDMA_IRQHandler              MDMA_IRQHandler

int  SDRAM_Init(void);


#endif /* __DRV_SDRAM_H */
