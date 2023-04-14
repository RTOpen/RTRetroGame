/*
 * COPYRIGHT (C) 2012-2020, Shanghai Real-Thread Technology Co., Ltd
 * All rights reserved.
 * Change Logs:
 * Date           Author       Notes
 * 2018-04-12     RT-Thread    the first version
 */

#include "board.h"
#include <rthw.h>
#include <string.h>
#include "hw_config.h"
#include "drv_lcd.h"

#ifdef BSP_USING_LCD

#if BSP_LCD_MODE_LUT8
uint8_t framebuffer1[BSP_LCD_WIDTH * BSP_LCD_HEIGHT];
uint8_t framebuffer2[BSP_LCD_WIDTH * BSP_LCD_HEIGHT];
#else
uint16_t framebuffer1[BSP_LCD_WIDTH * BSP_LCD_HEIGHT];
uint16_t framebuffer2[BSP_LCD_WIDTH * BSP_LCD_HEIGHT];
#endif // GW_LCD_MODE_LUT8

uint16_t *fb1 = framebuffer1;
uint16_t *fb2 = framebuffer2;

uint8_t emulator_framebuffer[(256 + 8 + 8) * 240];

extern LTDC_HandleTypeDef hltdc;

extern DAC_HandleTypeDef hdac1;
extern DAC_HandleTypeDef hdac2;

uint32_t active_framebuffer;
uint32_t frame_counter;

void lcd_backlight_off()
{
  HAL_DAC_Stop(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Stop(&hdac1, DAC_CHANNEL_2);
  HAL_DAC_Stop(&hdac2, DAC_CHANNEL_1);
}

void lcd_backlight_set(uint8_t brightness)
{
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_8B_R, brightness);
  HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_2, DAC_ALIGN_8B_R, brightness);
  HAL_DAC_SetValue(&hdac2, DAC_CHANNEL_1, DAC_ALIGN_8B_R, brightness);

  HAL_DAC_Start(&hdac1, DAC_CHANNEL_1);
  HAL_DAC_Start(&hdac1, DAC_CHANNEL_2);
  HAL_DAC_Start(&hdac2, DAC_CHANNEL_1);
}

void lcd_backlight_on()
{
  lcd_backlight_set(255);
}

static void lcd_set_power_1V8(uint32_t p) {
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, p == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void lcd_set_power_3V3(uint32_t p) {
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, p == 1 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void lcd_set_chipselect(uint32_t p) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, p == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
static void lcd_set_reset(uint32_t p) {
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, p == 0 ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void lcd_spi_tx(SPI_HandleTypeDef *spi, uint8_t *pData) {
  lcd_set_chipselect(1);
  HAL_Delay(2);
  HAL_SPI_Transmit(spi, pData, 2, 100);
  HAL_Delay(2);
  lcd_set_chipselect(0);
  HAL_Delay(2);
}

void lcd_deinit(SPI_HandleTypeDef *spi) {
  // Power off
  lcd_set_power_1V8(0);
  lcd_set_power_3V3(0);
}
static void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(hltdc.Instance==LTDC)
  {
  /* USER CODE BEGIN LTDC_MspInit 0 */

  /* USER CODE END LTDC_MspInit 0 */
    /* Peripheral clock enable */
    __HAL_RCC_LTDC_CLK_ENABLE();

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_13|GPIO_PIN_15;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_14|GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_3|GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF14_LTDC;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF10_LTDC;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF12_LTDC;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF9_LTDC;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF11_LTDC;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* LTDC interrupt Init */
    HAL_NVIC_SetPriority(LTDC_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(LTDC_IRQn);
  }

}
void lcd_init(SPI_HandleTypeDef *spi, LTDC_HandleTypeDef *ltdc) {
  // Disable LCD Chip select
  lcd_set_chipselect(0);

  // LCD reset
  lcd_set_reset(1);

  // Wake up !
  // Enable 1.8V &3V3 power supply
  lcd_set_power_3V3(1);
  lcd_set_power_1V8(1);
  HAL_Delay(20);

  // Lets go, bootup sequence.
  /* reset sequence */
  lcd_set_reset(0);
  HAL_Delay(1);
  lcd_set_reset(1);
  HAL_Delay(15);
  lcd_set_reset(0);
  HAL_Delay(1);

  lcd_spi_tx(spi, (uint8_t *)"\x08\x80");
  lcd_spi_tx(spi, (uint8_t *)"\x6E\x80");
  lcd_spi_tx(spi, (uint8_t *)"\x80\x80");

  // change x00 one of those lines to flip the screen :)
  lcd_spi_tx(spi, (uint8_t *)"\x68\x00");
  lcd_spi_tx(spi, (uint8_t *)"\xd0\x00");
  lcd_spi_tx(spi, (uint8_t *)"\x1b\x00");
  lcd_spi_tx(spi, (uint8_t *)"\xe0\x00");

  lcd_spi_tx(spi, (uint8_t *)"\x6a\x80");
  lcd_spi_tx(spi, (uint8_t *)"\x80\x00");
  lcd_spi_tx(spi, (uint8_t *)"\x14\x80");


  HAL_LTDC_SetAddress(ltdc,(uint32_t) &fb1, 0);

  memset(fb1, 0, sizeof(framebuffer1));
  memset(fb2, 0, sizeof(framebuffer1));
}

void HAL_LTDC_ReloadEventCallback (LTDC_HandleTypeDef *hltdc) {
  frame_counter++;
  if (active_framebuffer == 0) {
    HAL_LTDC_SetAddress(hltdc, (uint32_t) fb2, 0);
  } else {
    HAL_LTDC_SetAddress(hltdc, (uint32_t) fb1, 0);
  }
}

uint32_t is_lcd_swap_pending(void)
{
  return (uint32_t) ((hltdc.Instance->SRCR) & (LTDC_SRCR_VBR | LTDC_SRCR_IMR));
}

void lcd_swap(void)
{
  HAL_LTDC_Reload(&hltdc, LTDC_RELOAD_VERTICAL_BLANKING);
  active_framebuffer = active_framebuffer ? 0 : 1;
}

void lcd_sync(void)
{
  void *active = lcd_get_active_buffer();
  void *inactive = lcd_get_inactive_buffer();

  if (active != inactive) {
    memcpy(inactive, active, sizeof(framebuffer1));
  }
}

void* lcd_get_active_buffer(void)
{
  return active_framebuffer ? fb2 : fb1;
}

void* lcd_get_inactive_buffer(void)
{
  return active_framebuffer ? fb1 : fb2;
}

void lcd_reset_active_buffer(void)
{
  HAL_LTDC_SetAddress(&hltdc, (uint32_t) fb1, 0);
  active_framebuffer = 0;
}

void lcd_set_buffers(uint16_t *buf1, uint16_t *buf2)
{
  fb1 = buf1;
  fb2 = buf2;
}

void lcd_wait_for_vblank(void)
{
  uint32_t old_counter = frame_counter;
  while (old_counter == frame_counter) {
    __asm("nop");
  }
}


#endif
