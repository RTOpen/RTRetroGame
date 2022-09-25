#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#define LED_GPIO_RCC               (LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOA)
#define LED_RED_PORT               GPIOB
#define LED_RED_PIN                LL_GPIO_PIN_0
#define LED_GREEN_PORT             GPIOB
#define LED_GREEN_PIN              LL_GPIO_PIN_1
#define LED_BLUE_PORT              GPIOA
#define LED_BLUE_PIN               LL_GPIO_PIN_3

#define UART1_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOA
#define UART1_TX_PORT              GPIOA
#define UART1_TX_PIN               LL_GPIO_PIN_9
#define UART1_TX_AF                LL_GPIO_AF_7
#define UART1_RX_PORT              GPIOA
#define UART1_RX_PIN               LL_GPIO_PIN_10
#define UART1_RX_AF                LL_GPIO_AF_7
#define UART1_DMA                  DMA1
#define UART1_RX_DMA_ST            LL_DMA_STREAM_1
#define UART1_RX_DMA_REQ           LL_DMAMUX1_REQ_USART1_RX
#define UART1_TX_DMA_ST            LL_DMA_STREAM_0
#define UART1_TX_DMA_REQ           LL_DMAMUX1_REQ_USART1_TX
#define UART1_RX_DMA_Handler       DMA1_Stream1_IRQHandler
#define UART1_RX_IRQn              DMA1_Stream1_IRQn
#define UART1_TX_DMA_Handler       DMA1_Stream0_IRQHandler
#define UART1_TX_IRQn              DMA1_Stream0_IRQn
#define UART1_DMA_RX_CLEAR_FLAG()  UART1_DMA->LIFCR = DMA_LIFCR_CTCIF1|DMA_LIFCR_CTEIF1
#define UART1_DMA_TX_CLEAR_FLAG()  UART1_DMA->LIFCR = DMA_LIFCR_CTCIF0|DMA_LIFCR_CTEIF0


/**OCTOSPI1 GPIO Configuration
PE11     ------> OCTOSPIM_P1_NCS
PB2      ------> OCTOSPIM_P1_CLK
PB1      ------> OCTOSPIM_P1_IO0
PD12     ------> OCTOSPIM_P1_IO1
PE2      ------> OCTOSPIM_P1_IO2
PA1      ------> OCTOSPIM_P1_IO3

 */
#define OSPI_GPIO_RCC              (LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOD |LL_AHB4_GRP1_PERIPH_GPIOE)
#define OSPI_CS_PORT               GPIOB
#define OSPI_CS_PIN                LL_GPIO_PIN_6
#define OSPI_CS_AF                 LL_GPIO_AF_10
#define OSPI_CLK_PORT              GPIOB
#define OSPI_CLK_PIN               LL_GPIO_PIN_2
#define OSPI_CLK_AF                LL_GPIO_AF_9
#define OSPI_BK1_IO0_PORT          GPIOD
#define OSPI_BK1_IO0_PIN           LL_GPIO_PIN_11
#define OSPI_BK1_IO0_AF            LL_GPIO_AF_9
#define OSPI_BK1_IO1_PORT          GPIOD
#define OSPI_BK1_IO1_PIN           LL_GPIO_PIN_12
#define OSPI_BK1_IO1_AF            LL_GPIO_AF_9
#define OSPI_BK1_IO2_PORT          GPIOE
#define OSPI_BK1_IO2_PIN           LL_GPIO_PIN_2
#define OSPI_BK1_IO2_AF            LL_GPIO_AF_9
#define OSPI_BK1_IO3_PORT          GPIOD
#define OSPI_BK1_IO3_PIN           LL_GPIO_PIN_13
#define OSPI_BK1_IO3_AF            LL_GPIO_AF_9


/** LTDC GPIO
PI15    ------> LTDC_R0
PJ0     ------> LTDC_R1
PJ1     ------> LTDC_R2
PJ2     ------> LTDC_R3
PJ3     ------> LTDC_R4
PJ4     ------> LTDC_R5
PJ5     ------> LTDC_R6
PJ6     ------> LTDC_R7
PJ7     ------> LTDC_G0
PJ8     ------> LTDC_G1
PJ9     ------> LTDC_G2
PJ10    ------> LTDC_G3
PJ11    ------> LTDC_G4
PK0     ------> LTDC_G5
PK1     ------> LTDC_G6
PK2     ------> LTDC_G7
PJ12    ------> LTDC_B0
PJ13    ------> LTDC_B1
PJ14    ------> LTDC_B2
PJ15    ------> LTDC_B3
PK3     ------> LTDC_B4
PK4     ------> LTDC_B5
PK5     ------> LTDC_B6
PK6     ------> LTDC_B7
PI14    ------> LTDC_CLK
PI12    ------> LTDC_HSYNC
PI13    ------> LTDC_VSYNC
PK7     ------> LTDC_DE
PD13    ------> LCD_BL
*/
#define LTDC_GPIO_RCC              (LL_AHB4_GRP1_PERIPH_GPIOD | LL_AHB4_GRP1_PERIPH_GPIOI | LL_AHB4_GRP1_PERIPH_GPIOJ | LL_AHB4_GRP1_PERIPH_GPIOK)
#define LTDC_R0_PORT                GPIOI
#define LTDC_R0_PIN                 LL_GPIO_PIN_15
#define LTDC_R0_AF                  GPIO_AF14_LTDC
#define LTDC_R1_PORT                GPIOJ
#define LTDC_R1_PIN                 LL_GPIO_PIN_0
#define LTDC_R1_AF                  GPIO_AF14_LTDC
#define LTDC_R2_PORT                GPIOJ
#define LTDC_R2_PIN                 LL_GPIO_PIN_1
#define LTDC_R2_AF                  GPIO_AF14_LTDC
#define LTDC_R3_PORT                GPIOJ
#define LTDC_R3_PIN                 LL_GPIO_PIN_2
#define LTDC_R3_AF                  GPIO_AF14_LTDC
#define LTDC_R4_PORT                GPIOJ
#define LTDC_R4_PIN                 LL_GPIO_PIN_3
#define LTDC_R4_AF                  GPIO_AF14_LTDC
#define LTDC_R5_PORT                GPIOJ
#define LTDC_R5_PIN                 LL_GPIO_PIN_4
#define LTDC_R5_AF                  GPIO_AF14_LTDC
#define LTDC_R6_PORT                GPIOJ
#define LTDC_R6_PIN                 LL_GPIO_PIN_5
#define LTDC_R6_AF                  GPIO_AF14_LTDC
#define LTDC_R7_PORT                GPIOJ
#define LTDC_R7_PIN                 LL_GPIO_PIN_6
#define LTDC_R7_AF                  GPIO_AF14_LTDC
//绿色数据线
#define LTDC_G0_PORT                GPIOJ
#define LTDC_G0_PIN                 LL_GPIO_PIN_7
#define LTDC_G0_AF                  GPIO_AF14_LTDC
#define LTDC_G1_PORT                GPIOJ
#define LTDC_G1_PIN                 LL_GPIO_PIN_8
#define LTDC_G1_AF                  GPIO_AF14_LTDC
#define LTDC_G2_PORT                GPIOJ
#define LTDC_G2_PIN                 LL_GPIO_PIN_9
#define LTDC_G2_AF                  GPIO_AF14_LTDC
#define LTDC_G3_PORT                GPIOJ
#define LTDC_G3_PIN                 LL_GPIO_PIN_10
#define LTDC_G3_AF                  GPIO_AF14_LTDC
#define LTDC_G4_PORT                GPIOJ
#define LTDC_G4_PIN                 LL_GPIO_PIN_11
#define LTDC_G4_AF                  GPIO_AF14_LTDC
#define LTDC_G5_PORT                GPIOK
#define LTDC_G5_PIN                 LL_GPIO_PIN_0
#define LTDC_G5_AF                  GPIO_AF14_LTDC
#define LTDC_G6_PORT                GPIOK
#define LTDC_G6_PIN                 LL_GPIO_PIN_1
#define LTDC_G6_AF                  GPIO_AF14_LTDC
#define LTDC_G7_PORT                GPIOK
#define LTDC_G7_PIN                 LL_GPIO_PIN_2
#define LTDC_G7_AF                  GPIO_AF14_LTDC
//蓝色数据线
#define LTDC_B0_PORT                GPIOJ
#define LTDC_B0_PIN                 LL_GPIO_PIN_12
#define LTDC_B0_AF                  GPIO_AF14_LTDC
#define LTDC_B1_PORT                GPIOJ
#define LTDC_B1_PIN                 LL_GPIO_PIN_13
#define LTDC_B1_AF                  GPIO_AF14_LTDC
#define LTDC_B2_PORT                GPIOJ
#define LTDC_B2_PIN                 LL_GPIO_PIN_14
#define LTDC_B2_AF                  GPIO_AF14_LTDC
#define LTDC_B3_PORT                GPIOJ
#define LTDC_B3_PIN                 LL_GPIO_PIN_15
#define LTDC_B3_AF                  GPIO_AF14_LTDC
#define LTDC_B4_PORT                GPIOK
#define LTDC_B4_PIN                 LL_GPIO_PIN_3
#define LTDC_B4_AF                  GPIO_AF14_LTDC
#define LTDC_B5_PORT                GPIOK
#define LTDC_B5_PIN                 LL_GPIO_PIN_4
#define LTDC_B5_AF                  GPIO_AF14_LTDC
#define LTDC_B6_PORT                GPIOK
#define LTDC_B6_PIN                 LL_GPIO_PIN_5
#define LTDC_B6_AF                  GPIO_AF14_LTDC
#define LTDC_B7_PORT                GPIOK
#define LTDC_B7_PIN                 LL_GPIO_PIN_6
#define LTDC_B7_AF                  GPIO_AF14_LTDC
//控制信号线
/*像素时钟CLK*/
#define LTDC_CLK_PORT               GPIOI
#define LTDC_CLK_PIN                LL_GPIO_PIN_14
#define LTDC_CLK_AF                 GPIO_AF14_LTDC
/*水平同步信号HSYNC*/
#define LTDC_HSYNC_PORT             GPIOI
#define LTDC_HSYNC_PIN              LL_GPIO_PIN_12
#define LTDC_HSYNC_AF               GPIO_AF14_LTDC
/*垂直同步信号VSYNC*/
#define LTDC_VSYNC_PORT             GPIOI
#define LTDC_VSYNC_PIN              LL_GPIO_PIN_13
#define LTDC_VSYNC_AF               GPIO_AF14_LTDC
/*数据使能信号DE*/
#define LTDC_DE_PORT                GPIOK
#define LTDC_DE_PIN                 LL_GPIO_PIN_7
#define LTDC_DE_AF                  GPIO_AF14_LTDC
/*液晶屏背光信号，高电平使能*/
#define LTDC_BL_PORT                GPIOD
#define LTDC_BL_PIN                 LL_GPIO_PIN_13


/**
*  BUTTONS GPIO Configuration
PD11     ------> BUTTON_LEFT
PD0      ------> BUTTON_UP
PD15     ------> BUTTON_RIGHT
PD14     ------> BUTTON_DOWN
PD9      ------> BUTTON_A
PD5      ------> BUTTON_B
PC5      ------> BUTTON_TIME
PC1      ------> BUTTON_GAME
PC11     ------> BUTTON_START
PC12     ------> BUTTON_SELECT
PC13     ------> BUTTON_PAUSE
PA0      ------> BUTTON_POWER
*/
#define BUTTONS_GPIO_RCC  (LL_AHB4_GRP1_PERIPH_GPIOA | LL_AHB4_GRP1_PERIPH_GPIOC | LL_AHB4_GRP1_PERIPH_GPIOD)
#define BUTTON_LEFT_GPIO  GPIOD
#define BUTTON_LEFT_PIN   LL_GPIO_PIN_11
#define BUTTON_UP_GPIO    GPIOD
#define BUTTON_UP_PIN     LL_GPIO_PIN_0
#define BUTTON_RIGHT_GPIO GPIOD
#define BUTTON_RIGHT_PIN  LL_GPIO_PIN_15
#define BUTTON_DOWN_GPIO  GPIOD
#define BUTTON_DOWN_PIN   LL_GPIO_PIN_14
#define BUTTON_A_GPIO     GPIOD
#define BUTTON_A_PIN      LL_GPIO_PIN_9
#define BUTTON_B_GPIO     GPIOD
#define BUTTON_B_PIN      LL_GPIO_PIN_5
#define BUTTON_TIME_GPIO  GPIOC
#define BUTTON_TIME_PIN   LL_GPIO_PIN_5
#define BUTTON_GAME_GPIO  GPIOC
#define BUTTON_GAME_PIN   LL_GPIO_PIN_1
#define BUTTON_PAUSE_GPIO GPIOC
#define BUTTON_PAUSE_PIN  LL_GPIO_PIN_13
#define BUTTON_POWER_GPIO GPIOA
#define BUTTON_POWER_PIN  LL_GPIO_PIN_0
// Zelda only buttons; they are not connected on mario.
#define BUTTON_START_GPIO GPIOC
#define BUTTON_START_PIN  LL_GPIO_PIN_11
#define BUTTON_SELECT_GPIO GPIOC
#define BUTTON_SELECT_PIN LL_GPIO_PIN_12


#endif
