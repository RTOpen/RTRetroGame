#include "rtthread.h"
#include "drv_led.h"
#include "hw_config.h"


int led_hw_init(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
	LL_GPIO_StructInit(&GPIO_InitStruct);
	LL_AHB4_GRP1_EnableClock(LED_GPIO_RCC);
	/* Configure IO in output push-pull mode to drive external LED */
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
	GPIO_InitStruct.Pin = LED_RED_PIN;
    LL_GPIO_Init(LED_RED_PORT,&GPIO_InitStruct);
 	GPIO_InitStruct.Pin = LED_GREEN_PIN;
    LL_GPIO_Init(LED_GREEN_PORT,&GPIO_InitStruct);
	GPIO_InitStruct.Pin = LED_BLUE_PIN;
    LL_GPIO_Init(LED_BLUE_PORT,&GPIO_InitStruct); 
	GPIO_InitStruct.Pin = LED_YELLOW_PIN;
    LL_GPIO_Init(LED_YELLOW_PORT,&GPIO_InitStruct); 
	/* turn off all led */
	led_off(LED_ALL);
  
	return 0;
}
INIT_BOARD_EXPORT(led_hw_init);

void led_on(uint8_t leds)
{
	if(leds&LED_RED)
	{
		LED_RED_PORT->BSRR  = (LED_RED_PIN << 16);
	}
	if(leds&LED_GREEN)
	{
		LED_GREEN_PORT->BSRR  = (LED_GREEN_PIN << 16);
	}
	if(leds&LED_BLUE)
	{
		LED_BLUE_PORT->BSRR  = (LED_BLUE_PIN << 16);
	}
	if(leds&LED_YELLOW)
	{
		LED_YELLOW_PORT->BSRR  = (LED_YELLOW_PIN << 16);
	}
}

void led_off(uint8_t leds)
{
	if(leds&LED_RED)
	{
		LED_RED_PORT->BSRR  = LED_RED_PIN;
	}
	if(leds&LED_GREEN)
	{
		LED_GREEN_PORT->BSRR  = LED_GREEN_PIN;
	}
	if(leds&LED_BLUE)
	{
		LED_BLUE_PORT->BSRR  = LED_BLUE_PIN;
	}
	if(leds&LED_YELLOW)
	{
		LED_YELLOW_PORT->BSRR  = LED_YELLOW_PIN;
	}
}

void led_toggle(uint8_t leds)
{
	if(leds&LED_RED)
	{
		LED_RED_PORT->ODR  ^= LED_RED_PIN;
	}
	if(leds&LED_GREEN)
	{
		LED_GREEN_PORT->ODR  ^= LED_GREEN_PIN;
	}
	if(leds&LED_BLUE)
	{
		LED_BLUE_PORT->ODR  ^= LED_BLUE_PIN;
	}
    if(leds&LED_YELLOW)
	{
		LED_YELLOW_PORT->ODR  ^= LED_YELLOW_PIN;
	}
}
