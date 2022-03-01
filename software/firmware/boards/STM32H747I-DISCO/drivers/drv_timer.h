#ifndef __DRV_TIMER_H
#define __DRV_TIMER_H
#include "stm32h7xx.h"


#define TIMER_HRT_TIM                 TIM2    // can only use 32bit timers TIM2 or TIM5
#define TIMER_HRT_TIM_RCC             LL_APB1_GRP1_PERIPH_TIM2
#define TIMER_HRT_TIM_IRQ             TIM2_IRQn
#define TIMER_HRT_TIM_IRQHandler      TIM2_IRQHandler
#define TIMER_HRT_CORE_HALT           DBGMCU_TIM2_STOP

#define timer_micros()                TIMER_HRT_TIM->CNT

typedef enum
{
    TIMER_HRT_CH1,
    TIMER_HRT_CH2,
    TIMER_HRT_CH3,
    TIMER_HRT_CH4,
    TIMER_HRT_MAX,
} hrt_ch_t;

typedef struct
{
    void (*timeout)(void *parameter);
    void *parameter;
} timer_alarm_t;

int  timer_hw_init(void);

void  timer_delay_us(uint16_t us);

void timer_set_alarm(uint8_t ch, void (*timeout)(void *parameter), void *parameter);

void timer_alarm_start(uint8_t ch, uint32_t us);

void timer_alarm_stop(uint8_t ch);

#endif
