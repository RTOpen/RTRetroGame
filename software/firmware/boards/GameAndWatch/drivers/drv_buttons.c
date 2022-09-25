#include "stm32h7xx.h"
#include "drv_buttons.h"
#include "hw_config.h"

int buttons_hw_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    /* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(BUTTONS_GPIO_RCC);
    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Pin = BUTTON_LEFT_PIN;
    LL_GPIO_Init(BUTTON_LEFT_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_UP_PIN;
    LL_GPIO_Init(BUTTON_UP_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_RIGHT_PIN;
    LL_GPIO_Init(BUTTON_RIGHT_GPIO, &GPIO_InitStruct);    
    GPIO_InitStruct.Pin = BUTTON_DOWN_PIN;
    LL_GPIO_Init(BUTTON_DOWN_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_A_PIN;
    LL_GPIO_Init(BUTTON_A_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_B_PIN;
    LL_GPIO_Init(BUTTON_B_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_TIME_PIN;
    LL_GPIO_Init(BUTTON_TIME_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_GAME_PIN;
    LL_GPIO_Init(BUTTON_GAME_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_PAUSE_PIN;
    LL_GPIO_Init(BUTTON_PAUSE_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_POWER_PIN;
    LL_GPIO_Init(BUTTON_POWER_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_START_PIN;
    LL_GPIO_Init(BUTTON_START_GPIO, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = BUTTON_SELECT_PIN;
    LL_GPIO_Init(BUTTON_SELECT_GPIO, &GPIO_InitStruct);
    
    return 0;
}
uint32_t buttons_scan(void) {

    uint32_t value = 0;

    if((BUTTON_LEFT_GPIO->IDR&BUTTON_LEFT_PIN)==0)
    {
       value |= BUTTON_ID_LEFT;
    }
    if((BUTTON_UP_GPIO->IDR&BUTTON_UP_PIN)==0)
    {
       value |= BUTTON_ID_UP;
    }
    if((BUTTON_RIGHT_GPIO->IDR&BUTTON_RIGHT_PIN)==0)
    {
       value |= BUTTON_ID_RIGHT;
    }
    if((BUTTON_DOWN_GPIO->IDR&BUTTON_DOWN_PIN)==0)
    {
       value |= BUTTON_ID_DOWN;
    }
    if((BUTTON_A_GPIO->IDR&BUTTON_A_PIN)==0)
    {
       value |= BUTTON_ID_A;
    }
    if((BUTTON_B_GPIO->IDR&BUTTON_B_PIN)==0)
    {
       value |= BUTTON_ID_B;
    }
    if((BUTTON_TIME_GPIO->IDR&BUTTON_TIME_PIN)==0)
    {
       value |= BUTTON_ID_TIME;
    }
    if((BUTTON_GAME_GPIO->IDR&BUTTON_GAME_PIN)==0)
    {
       value |= BUTTON_ID_GAME;
    }
    if((BUTTON_PAUSE_GPIO->IDR&BUTTON_PAUSE_PIN)==0)
    {
       value |= BUTTON_ID_PAUSE;
    }
    if((BUTTON_POWER_GPIO->IDR&BUTTON_POWER_PIN)==0)
    {
       value |= BUTTON_ID_POWER;
    }
    if((BUTTON_START_GPIO->IDR&BUTTON_START_PIN)==0)
    {
       value |= BUTTON_ID_GAME;
    }
    if((BUTTON_SELECT_GPIO->IDR&BUTTON_SELECT_PIN)==0)
    {
       value |= BUTTON_ID_TIME;
    }

    return value;
}
