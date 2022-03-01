/*
 * Copyright (c) 2006-2020, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-11-05     xiaonong      first implementation
 */

#include <rtthread.h>
#include "board.h"
#include "drv_mpu.h"
#include "drv_led.h"
#include "drv_usart.h"

/**
 * @addtogroup STM32
 */

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE BYPASS)
  *            SYSCLK(Hz)                     = 400000000 (CPU Clock)
  *            HCLK(Hz)                       = 200000000 (AXI and AHBs Clock)
  *            AHB Prescaler                  = 2
  *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
  *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
  *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
  *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
  *            HSE Frequency(Hz)              = 25000000
  *            PLL_M                          = 5
  *            PLL_N                          = 160
  *            PLL_P                          = 2
  *            PLL_Q                          = 4
  *            PLL_R                          = 2
  *            VDD(V)                         = 3.3
  *            Flash Latency(WS)              = 4
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  /* Power configuration */
  LL_PWR_ConfigSupply(LL_PWR_DIRECT_SMPS_SUPPLY);
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  while(LL_PWR_IsActiveFlag_VOS() == 0) {}
  
  /* Enable HSE oscillator */
	//LL_RCC_HSE_EnableBypass();
  LL_RCC_HSE_Enable();
  while(LL_RCC_HSE_IsReady() != 1)
  {
  };

  /* Set FLASH latency */
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  /* Main PLL configuration and activation */
  LL_RCC_PLL_SetSource(LL_RCC_PLLSOURCE_HSE);
  LL_RCC_PLL1P_Enable();
  LL_RCC_PLL1Q_Enable();
  LL_RCC_PLL1R_Enable();
  LL_RCC_PLL1FRACN_Disable();
  LL_RCC_PLL1_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
  LL_RCC_PLL1_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL1_SetM(5);
  LL_RCC_PLL1_SetN(160);
  LL_RCC_PLL1_SetP(2);
  LL_RCC_PLL1_SetQ(4);
  LL_RCC_PLL1_SetR(2);
  LL_RCC_PLL1_Enable();
  while(LL_RCC_PLL1_IsReady() != 1)
  {
  };

  /* Set Sys & AHB & APB1 & APB2 & APB4  prescaler */
  LL_RCC_SetSysPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAHBPrescaler(LL_RCC_AHB_DIV_2);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_2);
  LL_RCC_SetAPB4Prescaler(LL_RCC_APB4_DIV_2);

  /* Set PLL1 as System Clock Source */
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL1);
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL1)
  {
  };

	/* Update CMSIS variable (which can be updated also through SystemCoreClockUpdate function) */
  SystemCoreClock = 400000000;
	
  /* Set systick to 1ms */
  SysTick_Config(SystemCoreClock / 1000);

	/* Select clock source for RNG peripheral */
  LL_RCC_SetRNGClockSource(LL_RCC_RNG_CLKSOURCE_PLL1Q);

  /* PLL3 configuration and activation */
  LL_RCC_PLL3P_Enable();
  LL_RCC_PLL3Q_Enable();
  LL_RCC_PLL3R_Enable();
	LL_RCC_PLL3FRACN_Disable();
  LL_RCC_PLL3_SetVCOInputRange(LL_RCC_PLLINPUTRANGE_2_4);
  LL_RCC_PLL3_SetVCOOutputRange(LL_RCC_PLLVCORANGE_WIDE);
  LL_RCC_PLL3_SetM(25);
  LL_RCC_PLL3_SetN(336);
  LL_RCC_PLL3_SetP(2);
  LL_RCC_PLL3_SetQ(7);
  LL_RCC_PLL3_SetR(2);
  LL_RCC_PLL3_Enable();
  while(LL_RCC_PLL3_IsReady() != 1)
  {
  };
  
	/* Select clock source for USB peripheral */
	LL_RCC_SetUSBClockSource(LL_RCC_USB_CLKSOURCE_PLL3Q);
  /* Select clock source for SDMMC peripheral */
  LL_RCC_SetSDMMCClockSource(LL_RCC_SDMMC_CLKSOURCE_PLL1Q);
  /* Enable CSI clock */
  LL_RCC_CSI_Enable();
  /* Enable SYSCFG clock */
  LL_APB4_GRP1_EnableClock(LL_APB4_GRP1_PERIPH_SYSCFG) ;
	
	/* Enables the I/O Compensation Cell */    
	LL_SYSCFG_EnableCompensationCell();
		
	/* Enable the CRC Module */
	LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_CRC); 
}

/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
    /* enter interrupt */
    rt_interrupt_enter();

    /* tick for HAL Library */
    HAL_IncTick();

    rt_tick_increase();

    /* leave interrupt */
    rt_interrupt_leave();
}

/* re-implementat tick interface for STM32 HAL */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
    /*Configure the SysTick to have interrupt in 1ms time basis*/
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);

    /*Configure the SysTick IRQ priority */
    NVIC_SetPriority(SysTick_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),
                              TickPriority, 0));

    /* Return function status */
    return HAL_OK;
}

void HAL_Delay(__IO uint32_t Delay)
{
   if(rt_thread_self()!=RT_NULL)
		{
    rt_thread_delay(Delay);
		}else
		{
		 while(Delay--)
		 {
		  uint32_t i = 1000000;
			while(i--);
		 }
		}
}

void HAL_SuspendTick(void)
{
    /* we should not suspend tick */
}

void HAL_ResumeTick(void)
{
    /* we should not resume tick */
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
    /* Configure the MPU attributes as Write Through */
    mpu_init();

    /* STM32H7xx HAL library initialization:
    - Configure the Flash ART accelerator on ITCM interface
    - Configure the Systick to generate an interrupt each 1 msec
    - Set NVIC Group Priority to 4
    - Global MSP (MCU Support Package) initialization
    */
    HAL_Init();
    /* Configure the system clock @ 400 Mhz */
    SystemClock_Config();
    /* init systick */
    SysTick_Config(SystemCoreClock / RT_TICK_PER_SECOND);
    /* set pend exception priority */
    NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
    rt_system_heap_init((void*)HEAP_BEGIN, (void*)HEAP_END);

#ifdef RT_USING_CONSOLE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
}

