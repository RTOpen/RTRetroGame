#ifndef __DRV_LED_H
#define __DRV_LED_H

#include "stm32h7xx.h"

#define LED_RED         0x01
#define LED_GREEN       0x02
#define LED_BLUE        0x04
#define LED_YELLOW      0x08
#define LED_ALL         0x0f

int led_hw_init(void);
void led_on(uint8_t leds);
void led_off(uint8_t leds);
void led_toggle(uint8_t leds);

#endif
