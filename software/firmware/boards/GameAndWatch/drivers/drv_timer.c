#include "rtthread.h"
#include "drv_timer.h"

static timer_alarm_t timer_alarm[TIMER_HRT_MAX];

static void NVIC_Configuration(void)
{
 /***********************************************/
  /* Configure the NVIC to handle TIM2 interrupt */
  /***********************************************/
  NVIC_SetPriority(TIM2_IRQn, 0);
  NVIC_EnableIRQ(TIM2_IRQn);
}

static void TIM_Configuration(void)
{
    LL_TIM_OC_InitTypeDef  TIM_OCInitStructure;
    LL_TIM_InitTypeDef  TIM_InitStructure;
    /* Enable the timer peripheral clock */
    LL_APB1_GRP1_EnableClock(TIMER_HRT_TIM_RCC); 
	
  /***************************/
  /* Time base configuration */
  /***************************/
  LL_TIM_StructInit(&TIM_InitStructure);
  
    /* Set the pre-scaler value to have TIMER_HRT_TIM counter clock equal to 1 MHz */
    TIM_InitStructure.Prescaler =__LL_TIM_CALC_PSC(SystemCoreClock/2, 1000000);
  
    LL_TIM_Init(TIMER_HRT_TIM,&TIM_InitStructure);
    
    // Output Compare for alarms
    LL_TIM_OC_StructInit(&TIM_OCInitStructure);
    TIM_OCInitStructure.OCMode = LL_TIM_OCMODE_INACTIVE;
    TIM_OCInitStructure.OCState = LL_TIM_OCSTATE_ENABLE;
    TIM_OCInitStructure.OCPolarity = LL_TIM_OCPOLARITY_HIGH;

    LL_TIM_OC_Init(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH1,&TIM_OCInitStructure);
		LL_TIM_OC_DisablePreload(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH1);

    LL_TIM_OC_Init(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH2,&TIM_OCInitStructure);
    LL_TIM_OC_DisablePreload(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH2);

    LL_TIM_OC_Init(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH3,&TIM_OCInitStructure);
    LL_TIM_OC_DisablePreload(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH3);

    LL_TIM_OC_Init(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH4,&TIM_OCInitStructure);
    LL_TIM_OC_DisablePreload(TIMER_HRT_TIM, LL_TIM_CHANNEL_CH4);

    /* Enable counter */
    LL_TIM_EnableCounter(TIMER_HRT_TIM);
  
    /* Force update generation */
    LL_TIM_GenerateEvent_UPDATE(TIMER_HRT_TIM);

}

int  timer_hw_init(void)
{
    uint8_t i = 0;
    for (i = TIMER_HRT_CH1; i < TIMER_HRT_MAX; i++)
    {
        timer_alarm[i].timeout = NULL;
        timer_alarm[i].parameter = NULL;
    }
    NVIC_Configuration();
    TIM_Configuration();

    return 0;
}
INIT_BOARD_EXPORT(timer_hw_init);

void  timer_delay_us(uint16_t us)
{
    if (us > 20)
    {
        uint32_t next = TIMER_HRT_TIM->CNT + us;
        /* disable interrupt */
        while (TIMER_HRT_TIM->CNT < next)
        {

        }
    }
    else
    {
        while (us--)
        {

        }
    }
}

void timer_set_alarm(uint8_t ch, void (*timeout)(void *parameter), void *parameter)
{
    if (ch < TIMER_HRT_MAX)
    {
        timer_alarm[ch].timeout = timeout;
        timer_alarm[ch].parameter = parameter;
    }
}
void timer_alarm_start(uint8_t ch, uint32_t us)
{
    rt_interrupt_enter();

    switch (ch)
    {
    case TIMER_HRT_CH1:
		    LL_TIM_ClearFlag_CC1(TIMER_HRT_TIM);
        TIMER_HRT_TIM->CCR1 = TIMER_HRT_TIM->CNT + us;
        TIMER_HRT_TIM->DIER |= TIM_IT_CC1;
        break;
    case TIMER_HRT_CH2:
        LL_TIM_ClearFlag_CC2(TIMER_HRT_TIM);
        TIMER_HRT_TIM->CCR2 = TIMER_HRT_TIM->CNT + us;
        TIMER_HRT_TIM->DIER |= TIM_IT_CC2;
        break;
    case TIMER_HRT_CH3:
        LL_TIM_ClearFlag_CC3(TIMER_HRT_TIM);
        TIMER_HRT_TIM->CCR3 = TIMER_HRT_TIM->CNT + us;
        TIMER_HRT_TIM->DIER |= TIM_IT_CC3;
        break;
    case TIMER_HRT_CH4:
        LL_TIM_ClearFlag_CC4(TIMER_HRT_TIM);
        TIMER_HRT_TIM->CCR4 = TIMER_HRT_TIM->CNT + us;
        TIMER_HRT_TIM->DIER |= TIM_IT_CC4;
        break;
    default:
        break;
    }
    rt_interrupt_leave();
}

void timer_alarm_stop(uint8_t ch)
{
    switch (ch)
    {
    case TIMER_HRT_CH1:
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= ~TIM_IT_CC1;
        break;
    case TIMER_HRT_CH2:
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= ~TIM_IT_CC2;
        break;
    case TIMER_HRT_CH3:
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= ~TIM_IT_CC3;
        break;
    case TIMER_HRT_CH4:
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= ~TIM_IT_CC4;
        break;
    default:
        break;
    }
}
void TIMER_HRT_TIM_IRQHandler(void)
{
    rt_interrupt_enter();
    if (LL_TIM_IsActiveFlag_UPDATE(TIMER_HRT_TIM) != RESET)
    {
        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_UPDATE(TIMER_HRT_TIM);
    }
    if (LL_TIM_IsActiveFlag_CC1(TIMER_HRT_TIM) != RESET)
    {
        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_CC1(TIMER_HRT_TIM);
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= (uint16_t)~TIM_IT_CC1;
        if (timer_alarm[TIMER_HRT_CH1].timeout != NULL)
        {
            timer_alarm[TIMER_HRT_CH1].timeout(timer_alarm[TIMER_HRT_CH1].parameter);
        }
    }
    if (LL_TIM_IsActiveFlag_CC2(TIMER_HRT_TIM) != RESET)
    {
        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_CC2(TIMER_HRT_TIM);
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= (uint16_t)~TIM_IT_CC2;
        if (timer_alarm[TIMER_HRT_CH2].timeout != NULL)
        {
            timer_alarm[TIMER_HRT_CH2].timeout(timer_alarm[TIMER_HRT_CH2].parameter);
        }
    }
    if (LL_TIM_IsActiveFlag_CC3(TIMER_HRT_TIM) != RESET)
    {
        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_CC3(TIMER_HRT_TIM);
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= (uint16_t)~TIM_IT_CC3;
        if (timer_alarm[TIMER_HRT_CH3].timeout != NULL)
        {
            timer_alarm[TIMER_HRT_CH3].timeout(timer_alarm[TIMER_HRT_CH3].parameter);
        }
    }
    if (LL_TIM_IsActiveFlag_CC4(TIMER_HRT_TIM) != RESET)
    {
        /* Clear the update interrupt flag*/
        LL_TIM_ClearFlag_CC4(TIMER_HRT_TIM);
        // Disable the Interrupt
        TIMER_HRT_TIM->DIER &= (uint16_t)~TIM_IT_CC4;
        if (timer_alarm[TIMER_HRT_CH4].timeout != NULL)
        {
            timer_alarm[TIMER_HRT_CH4].timeout(timer_alarm[TIMER_HRT_CH4].parameter);
        }
    }
    rt_interrupt_leave();
}
