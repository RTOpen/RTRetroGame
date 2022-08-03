#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#define LED_GPIO_RCC               LL_AHB4_GRP1_PERIPH_GPIOI
#define LED_GREEN_PORT             GPIOI
#define LED_GREEN_PIN              LL_GPIO_PIN_12
#define LED_YELLOW_PORT            GPIOI
#define LED_YELLOW_PIN             LL_GPIO_PIN_13
#define LED_RED_PORT               GPIOI
#define LED_RED_PIN                LL_GPIO_PIN_14
#define LED_BLUE_PORT              GPIOI
#define LED_BLUE_PIN               LL_GPIO_PIN_15

#define KEY_GPIO_RCC               (LL_AHB4_GRP1_PERIPH_GPIOC | LL_AHB4_GRP1_PERIPH_GPIOK)
#define KEY_OK_PORT                GPIOK
#define KEY_OK_PIN                 LL_GPIO_PIN_2
#define KEY_DOWN_PORT              GPIOK
#define KEY_DOWN_PIN               LL_GPIO_PIN_3
#define KEY_LEFT_PORT              GPIOK
#define KEY_LEFT_PIN               LL_GPIO_PIN_4
#define KEY_RIGHT_PORT             GPIOK
#define KEY_RIGHT_PIN              LL_GPIO_PIN_5
#define KEY_UP_PORT                GPIOK
#define KEY_UP_PIN                 LL_GPIO_PIN_6
#define KEY_HOME_PORT              GPIOC
#define KEY_HOME_PIN               LL_GPIO_PIN_13

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


#define UART2_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOD
#define UART2_TX_PORT              GPIOD
#define UART2_TX_PIN               LL_GPIO_PIN_5
#define UART2_TX_AF                LL_GPIO_AF_7
#define UART2_RX_PORT              GPIOD
#define UART2_RX_PIN               LL_GPIO_PIN_6
#define UART2_RX_AF                LL_GPIO_AF_7
#define UART2_DMA                  DMA1
#define UART2_RX_DMA_ST            LL_DMA_STREAM_3
#define UART2_RX_DMA_REQ           LL_DMAMUX1_REQ_USART2_RX
//#define UART2_TX_DMA_ST            LL_DMA_STREAM_2
#define UART2_TX_DMA_REQ           LL_DMAMUX1_REQ_USART2_TX
#define UART2_RX_DMA_Handler       DMA1_Stream3_IRQHandler
#define UART2_RX_IRQn              DMA1_Stream3_IRQn
#define UART2_TX_DMA_Handler       DMA1_Stream2_IRQHandler
#define UART2_TX_IRQn              DMA1_Stream2_IRQn
#define UART2_DMA_RX_CLEAR_FLAG()  UART2_DMA->LIFCR = DMA_LIFCR_CTCIF3|DMA_LIFCR_CTEIF3
#define UART2_DMA_TX_CLEAR_FLAG()  UART2_DMA->LIFCR = DMA_LIFCR_CTCIF2|DMA_LIFCR_CTEIF2


#define UART3_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOD
#define UART3_TX_PORT              GPIOD
#define UART3_TX_PIN               LL_GPIO_PIN_8
#define UART3_TX_AF                LL_GPIO_AF_7
#define UART3_RX_PORT              GPIOD
#define UART3_RX_PIN               LL_GPIO_PIN_9
#define UART3_RX_AF                LL_GPIO_AF_7
#define UART3_DMA                  DMA1
#define UART3_RX_DMA_ST            LL_DMA_STREAM_5
#define UART3_RX_DMA_REQ           LL_DMAMUX1_REQ_USART3_RX
#define UART3_TX_DMA_ST            LL_DMA_STREAM_4
#define UART3_TX_DMA_REQ           LL_DMAMUX1_REQ_USART3_TX
#define UART3_RX_DMA_Handler       DMA1_Stream5_IRQHandler
#define UART3_RX_IRQn              DMA1_Stream5_IRQn
#define UART3_TX_DMA_Handler       DMA1_Stream4_IRQHandler
#define UART3_TX_IRQn              DMA1_Stream4_IRQn
#define UART3_DMA_RX_CLEAR_FLAG()  UART3_DMA->HIFCR = DMA_HIFCR_CTCIF5|DMA_HIFCR_CTEIF5
#define UART3_DMA_TX_CLEAR_FLAG()  UART3_DMA->HIFCR = DMA_HIFCR_CTCIF4|DMA_HIFCR_CTEIF4


#define UART4_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOC
#define UART4_TX_PORT              GPIOC
#define UART4_TX_PIN               LL_GPIO_PIN_10
#define UART4_TX_AF                LL_GPIO_AF_8
#define UART4_RX_PORT              GPIOC
#define UART4_RX_PIN               LL_GPIO_PIN_11
#define UART4_RX_AF                LL_GPIO_AF_8
#define UART4_DMA                  DMA1
#define UART4_RX_DMA_ST            LL_DMA_STREAM_7
#define UART4_RX_DMA_REQ           LL_DMAMUX1_REQ_UART4_RX
#define UART4_TX_DMA_ST            LL_DMA_STREAM_6
#define UART4_TX_DMA_REQ           LL_DMAMUX1_REQ_UART4_TX
#define UART4_RX_DMA_Handler       DMA1_Stream7_IRQHandler
#define UART4_RX_IRQn              DMA1_Stream7_IRQn
#define UART4_TX_DMA_Handler       DMA1_Stream6_IRQHandler
#define UART4_TX_IRQn              DMA1_Stream6_IRQn
#define UART4_DMA_RX_CLEAR_FLAG()  UART4_DMA->HIFCR = DMA_HIFCR_CTCIF7|DMA_HIFCR_CTEIF7
#define UART4_DMA_TX_CLEAR_FLAG()  UART4_DMA->HIFCR = DMA_HIFCR_CTCIF6|DMA_HIFCR_CTEIF6


#define UART5_PORT_RCC             (LL_AHB4_GRP1_PERIPH_GPIOC | LL_AHB4_GRP1_PERIPH_GPIOD)
#define UART5_TX_PORT              GPIOC
#define UART5_TX_PIN               LL_GPIO_PIN_12
#define UART5_TX_AF                LL_GPIO_AF_8
#define UART5_RX_PORT              GPIOD
#define UART5_RX_PIN               LL_GPIO_PIN_2
#define UART5_RX_AF                LL_GPIO_AF_8
#define UART5_DMA                  DMA2
#define UART5_RX_DMA_ST            LL_DMA_STREAM_1
#define UART5_RX_DMA_REQ           LL_DMAMUX1_REQ_UART5_RX
//#define UART5_TX_DMA_ST            LL_DMA_STREAM_0
#define UART5_TX_DMA_REQ           LL_DMAMUX1_REQ_UART5_TX
#define UART5_RX_DMA_Handler       DMA2_Stream1_IRQHandler
#define UART5_RX_IRQn              DMA2_Stream1_IRQn
#define UART5_TX_DMA_Handler       DMA2_Stream0_IRQHandler
#define UART5_TX_IRQn              DMA2_Stream0_IRQn
#define UART5_DMA_RX_CLEAR_FLAG()  UART5_DMA->LIFCR = DMA_LIFCR_CTCIF1|DMA_LIFCR_CTEIF1
#define UART5_DMA_TX_CLEAR_FLAG()  UART5_DMA->LIFCR = DMA_LIFCR_CTCIF0|DMA_LIFCR_CTEIF0


#define UART6_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOC
#define UART6_TX_PORT              GPIOC
#define UART6_TX_PIN               LL_GPIO_PIN_6
#define UART6_TX_AF                LL_GPIO_AF_7
#define UART6_RX_PORT              GPIOC
#define UART6_RX_PIN               LL_GPIO_PIN_7
#define UART6_RX_AF                LL_GPIO_AF_7
#define UART6_DMA                  DMA2
#define UART6_RX_DMA_ST            LL_DMA_STREAM_3
#define UART6_RX_DMA_REQ           LL_DMAMUX1_REQ_USART6_RX
#define UART6_TX_DMA_ST            LL_DMA_STREAM_2
#define UART6_TX_DMA_REQ           LL_DMAMUX1_REQ_USART6_TX
#define UART6_RX_DMA_Handler       DMA2_Stream3_IRQHandler
#define UART6_RX_IRQn              DMA2_Stream3_IRQn
#define UART6_TX_DMA_Handler       DMA2_Stream2_IRQHandler
#define UART6_TX_IRQn              DMA2_Stream2_IRQn
#define UART6_DMA_RX_CLEAR_FLAG()  UART6_DMA->LIFCR = DMA_LIFCR_CTCIF3|DMA_LIFCR_CTEIF3
#define UART6_DMA_TX_CLEAR_FLAG()  UART6_DMA->LIFCR = DMA_LIFCR_CTCIF2|DMA_LIFCR_CTEIF2


#define UART7_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOE
#define UART7_TX_PORT              GPIOE
#define UART7_TX_PIN               LL_GPIO_PIN_8
#define UART7_TX_AF                LL_GPIO_AF_7
#define UART7_RX_PORT              GPIOE
#define UART7_RX_PIN               LL_GPIO_PIN_7
#define UART7_RX_AF                LL_GPIO_AF_7
#define UART7_DMA                  DMA2
#define UART7_RX_DMA_ST            LL_DMA_STREAM_5
#define UART7_RX_DMA_REQ           LL_DMAMUX1_REQ_UART7_RX
#define UART7_TX_DMA_ST            LL_DMA_STREAM_4
#define UART7_TX_DMA_REQ           LL_DMAMUX1_REQ_UART7_TX
#define UART7_RX_DMA_Handler       DMA2_Stream5_IRQHandler
#define UART7_RX_IRQn              DMA2_Stream5_IRQn
#define UART7_TX_DMA_Handler       DMA2_Stream4_IRQHandler
#define UART7_TX_IRQn              DMA2_Stream4_IRQn
#define UART7_DMA_RX_CLEAR_FLAG()  UART7_DMA->HIFCR = DMA_HIFCR_CTCIF5|DMA_HIFCR_CTEIF5
#define UART7_DMA_TX_CLEAR_FLAG()  UART7_DMA->HIFCR = DMA_HIFCR_CTCIF4|DMA_HIFCR_CTEIF4


#define UART8_PORT_RCC             LL_AHB4_GRP1_PERIPH_GPIOE
#define UART8_TX_PORT              GPIOE
#define UART8_TX_PIN               LL_GPIO_PIN_1
#define UART8_TX_AF                LL_GPIO_AF_8
#define UART8_RX_PORT              GPIOE
#define UART8_RX_PIN               LL_GPIO_PIN_0
#define UART8_RX_AF                LL_GPIO_AF_8
#define UART8_DMA                  DMA2
#define UART8_RX_DMA_ST            LL_DMA_STREAM_7
#define UART8_RX_DMA_REQ           LL_DMAMUX1_REQ_UART8_RX
//#define UART8_TX_DMA_ST            LL_DMA_STREAM_6
#define UART8_TX_DMA_REQ           LL_DMAMUX1_REQ_UART8_TX
#define UART8_RX_DMA_Handler       DMA2_Stream7_IRQHandler
#define UART8_RX_IRQn              DMA2_Stream7_IRQn
#define UART8_TX_DMA_Handler       DMA2_Stream6_IRQHandler
#define UART8_TX_IRQn              DMA2_Stream6_IRQn
#define UART8_DMA_RX_CLEAR_FLAG()  UART8_DMA->HIFCR = DMA_HIFCR_CTCIF7|DMA_HIFCR_CTEIF7
#define UART8_DMA_TX_CLEAR_FLAG()  UART8_DMA->HIFCR = DMA_HIFCR_CTCIF6|DMA_HIFCR_CTEIF6

 /** QUADSPI GPIO
 PB6     ------> QUADSPI_BK1_NCS 
 PB2     ------> QUADSPI_CLK
 PD11    ------> QUADSPI_BK1_IO0
 PD12    ------> QUADSPI_BK1_IO1
 PE2     ------> QUADSPI_BK1_IO2
 PD13    ------> QUADSPI_BK1_IO3
 */
#define QSPI_GPIO_RCC              (LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOD |LL_AHB4_GRP1_PERIPH_GPIOE)
#define QSPI_CS_PORT               GPIOB
#define QSPI_CS_PIN                LL_GPIO_PIN_6
#define QSPI_CS_AF                 LL_GPIO_AF_10
#define QSPI_CLK_PORT              GPIOB
#define QSPI_CLK_PIN               LL_GPIO_PIN_2
#define QSPI_CLK_AF                LL_GPIO_AF_9
#define QSPI_BK1_IO0_PORT          GPIOD
#define QSPI_BK1_IO0_PIN           LL_GPIO_PIN_11
#define QSPI_BK1_IO0_AF            LL_GPIO_AF_9
#define QSPI_BK1_IO1_PORT          GPIOD
#define QSPI_BK1_IO1_PIN           LL_GPIO_PIN_12
#define QSPI_BK1_IO1_AF            LL_GPIO_AF_9
#define QSPI_BK1_IO2_PORT          GPIOE
#define QSPI_BK1_IO2_PIN           LL_GPIO_PIN_2
#define QSPI_BK1_IO2_AF            LL_GPIO_AF_9
#define QSPI_BK1_IO3_PORT          GPIOD
#define QSPI_BK1_IO3_PIN           LL_GPIO_PIN_13
#define QSPI_BK1_IO3_AF            LL_GPIO_AF_9

/** ETH GPIO Configuration
PG11    ------> ETH_TX_EN
PG13    ------> ETH_TXD0
PG12    ------> ETH_TXD1
PC1     ------> ETH_MDC
PA2     ------> ETH_MDIO
PA1     ------> ETH_REF_CLK
PA7     ------> ETH_CRS_DV
PC4     ------> ETH_RXD0
PC5     ------> ETH_RXD1
*/
#define ETH_GPIO_RCC              (LL_AHB4_GRP1_PERIPH_GPIOA  | LL_AHB4_GRP1_PERIPH_GPIOC | LL_AHB4_GRP1_PERIPH_GPIOG)
#define ETH_TX_EN_PORT            GPIOG
#define ETH_TX_EN_PIN             LL_GPIO_PIN_11
#define ETH_TX_EN_AF              LL_GPIO_AF_11
#define ETH_TXD0_PORT             GPIOG
#define ETH_TXD0_PIN              LL_GPIO_PIN_13
#define ETH_TXD0_AF               LL_GPIO_AF_11
#define ETH_TXD1_PORT             GPIOG
#define ETH_TXD1_PIN              LL_GPIO_PIN_12
#define ETH_TXD1_AF               LL_GPIO_AF_11
#define ETH_MDC_PORT              GPIOC
#define ETH_MDC_PIN               LL_GPIO_PIN_1
#define ETH_MDC_AF                LL_GPIO_AF_11
#define ETH_MDIO_PORT             GPIOA
#define ETH_MDIO_PIN              LL_GPIO_PIN_2
#define ETH_MDIO_AF               LL_GPIO_AF_11
#define ETH_REF_CLK_PORT          GPIOA
#define ETH_REF_CLK_PIN           LL_GPIO_PIN_1
#define ETH_REF_CLK_AF            LL_GPIO_AF_11
#define ETH_CRS_DV_PORT           GPIOA
#define ETH_CRS_DV_PIN            LL_GPIO_PIN_7
#define ETH_CRS_DV_AF             LL_GPIO_AF_11
#define ETH_RXD0_PORT             GPIOC
#define ETH_RXD0_PIN              LL_GPIO_PIN_4
#define ETH_RXD0_AF               LL_GPIO_AF_11
#define ETH_RXD1_PORT             GPIOC
#define ETH_RXD1_PIN              LL_GPIO_PIN_5
#define ETH_RXD1_AF               LL_GPIO_AF_11

 /** LTDC GPIO
 PI14    ------> LTDC_CLK
 PI12    ------> LTDC_HSYNC
 PI13    ------> LTDC_VSYNC
 PK7     ------> LTDC_DE
 PD13    ------> LCD_BL
 */
/**
  * @brief LCD special pins
  */
/* LCD reset pin */
#define LCD_RESET_PIN                    GPIO_PIN_3
#define LCD_RESET_PULL                   GPIO_NOPULL
#define LCD_RESET_GPIO_PORT              GPIOG
#define LCD_RESET_GPIO_CLK_ENABLE()      __HAL_RCC_GPIOG_CLK_ENABLE()
#define LCD_RESET_GPIO_CLK_DISABLE()     __HAL_RCC_GPIOG_CLK_DISABLE()

/* LCD tearing effect pin */
#define LCD_TE_PIN                       GPIO_PIN_2
#define LCD_TE_GPIO_PORT                 GPIOJ
#define LCD_TE_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOJ_CLK_ENABLE()
#define LCD_TE_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOJ_CLK_DISABLE()

/* Back-light control pin */
#define LCD_BL_CTRL_PIN                  GPIO_PIN_12
#define LCD_BL_CTRL_GPIO_PORT            GPIOJ
#define LCD_BL_CTRL_GPIO_CLK_ENABLE()    __HAL_RCC_GPIOJ_CLK_ENABLE()
#define LCD_BL_CTRL_GPIO_CLK_DISABLE()   __HAL_RCC_GPIOJ_CLK_DISABLE()

#define FMC_GPIO_RCC                (LL_AHB4_GRP1_PERIPH_GPIOD | LL_AHB4_GRP1_PERIPH_GPIOE | LL_AHB4_GRP1_PERIPH_GPIOF | LL_AHB4_GRP1_PERIPH_GPIOG | LL_AHB4_GRP1_PERIPH_GPIOH | LL_AHB4_GRP1_PERIPH_GPIOI)  
#define FMC_A0_PORT                 GPIOF
#define FMC_A0_PIN                  LL_GPIO_PIN_0
#define FMC_A1_PORT                 GPIOF
#define FMC_A1_PIN                  LL_GPIO_PIN_1
#define FMC_A2_PORT                 GPIOF
#define FMC_A2_PIN                  LL_GPIO_PIN_2
#define FMC_A3_PORT                 GPIOF
#define FMC_A3_PIN                  LL_GPIO_PIN_3
#define FMC_A4_PORT                 GPIOF
#define FMC_A4_PIN                  LL_GPIO_PIN_4
#define FMC_A5_PORT                 GPIOF
#define FMC_A5_PIN                  LL_GPIO_PIN_5
#define FMC_A6_PORT                 GPIOF
#define FMC_A6_PIN                  LL_GPIO_PIN_12
#define FMC_A7_PORT                 GPIOF
#define FMC_A7_PIN                  LL_GPIO_PIN_13
#define FMC_A8_PORT                 GPIOF
#define FMC_A8_PIN                  LL_GPIO_PIN_14
#define FMC_A9_PORT                 GPIOF
#define FMC_A9_PIN                  LL_GPIO_PIN_15
#define FMC_A10_PORT                GPIOG
#define FMC_A10_PIN                 LL_GPIO_PIN_0
#define FMC_A11_PORT                GPIOG
#define FMC_A11_PIN                 LL_GPIO_PIN_1
#define FMC_A12_PORT                GPIOG
#define FMC_A12_PIN                 LL_GPIO_PIN_2
/*数据信号线*/
#define FMC_D0_PORT                 GPIOD
#define FMC_D0_PIN                  LL_GPIO_PIN_14
#define FMC_D1_PORT                 GPIOD
#define FMC_D1_PIN                  LL_GPIO_PIN_15
#define FMC_D2_PORT                 GPIOD
#define FMC_D2_PIN                  LL_GPIO_PIN_0
#define FMC_D3_PORT                 GPIOD
#define FMC_D3_PIN                  LL_GPIO_PIN_1
#define FMC_D4_PORT                 GPIOE
#define FMC_D4_PIN                  LL_GPIO_PIN_7
#define FMC_D5_PORT                 GPIOE
#define FMC_D5_PIN                  LL_GPIO_PIN_8
#define FMC_D6_PORT                 GPIOE
#define FMC_D6_PIN                  LL_GPIO_PIN_9
#define FMC_D7_PORT                 GPIOE
#define FMC_D7_PIN                  LL_GPIO_PIN_10
#define FMC_D8_PORT                 GPIOE
#define FMC_D8_PIN                  LL_GPIO_PIN_11
#define FMC_D9_PORT                 GPIOE
#define FMC_D9_PIN                  LL_GPIO_PIN_12
#define FMC_D10_PORT                GPIOE
#define FMC_D10_PIN                 LL_GPIO_PIN_13
#define FMC_D11_PORT                GPIOE
#define FMC_D11_PIN                 LL_GPIO_PIN_14
#define FMC_D12_PORT                GPIOE
#define FMC_D12_PIN                 LL_GPIO_PIN_15
#define FMC_D13_PORT                GPIOD
#define FMC_D13_PIN                 LL_GPIO_PIN_8
#define FMC_D14_PORT                GPIOD
#define FMC_D14_PIN                 LL_GPIO_PIN_9
#define FMC_D15_PORT                GPIOD
#define FMC_D15_PIN                 LL_GPIO_PIN_10
#define FMC_D16_PORT                GPIOH
#define FMC_D16_PIN                 LL_GPIO_PIN_8
#define FMC_D17_PORT                GPIOH
#define FMC_D17_PIN                 LL_GPIO_PIN_9
#define FMC_D18_PORT                GPIOH
#define FMC_D18_PIN                 LL_GPIO_PIN_10
#define FMC_D19_PORT                GPIOH
#define FMC_D19_PIN                 LL_GPIO_PIN_11
#define FMC_D20_PORT                GPIOH
#define FMC_D20_PIN                 LL_GPIO_PIN_12
#define FMC_D21_PORT                GPIOH
#define FMC_D21_PIN                 LL_GPIO_PIN_13
#define FMC_D22_PORT                GPIOH
#define FMC_D22_PIN                 LL_GPIO_PIN_14
#define FMC_D23_PORT                GPIOH
#define FMC_D23_PIN                 LL_GPIO_PIN_15
#define FMC_D24_PORT                GPIOI
#define FMC_D24_PIN                 LL_GPIO_PIN_0
#define FMC_D25_PORT                GPIOI
#define FMC_D25_PIN                 LL_GPIO_PIN_1
#define FMC_D26_PORT                GPIOI
#define FMC_D26_PIN                 LL_GPIO_PIN_2
#define FMC_D27_PORT                GPIOI
#define FMC_D27_PIN                 LL_GPIO_PIN_3
#define FMC_D28_PORT                GPIOI
#define FMC_D28_PIN                 LL_GPIO_PIN_6
#define FMC_D29_PORT                GPIOI
#define FMC_D29_PIN                 LL_GPIO_PIN_7
#define FMC_D30_PORT                GPIOI
#define FMC_D30_PIN                 LL_GPIO_PIN_9
#define FMC_D31_PORT                GPIOI
#define FMC_D31_PIN                 LL_GPIO_PIN_10
/*控制信号线*/  
#define FMC_CS_PORT                 GPIOH
#define FMC_CS_PIN                  LL_GPIO_PIN_6
#define FMC_BA0_PORT                GPIOG
#define FMC_BA0_PIN                 LL_GPIO_PIN_4
#define FMC_BA1_PORT                GPIOG
#define FMC_BA1_PIN                 LL_GPIO_PIN_5
#define FMC_WE_PORT                 GPIOH
#define FMC_WE_PIN                  LL_GPIO_PIN_5
#define FMC_RAS_PORT                GPIOF
#define FMC_RAS_PIN                 LL_GPIO_PIN_11
#define FMC_CAS_PORT                GPIOG
#define FMC_CAS_PIN                 LL_GPIO_PIN_15
#define FMC_CLK_PORT                GPIOG
#define FMC_CLK_PIN                 LL_GPIO_PIN_8
#define FMC_CKE_PORT                GPIOH
#define FMC_CKE_PIN                 LL_GPIO_PIN_7
/*DQM0 DQM1 DQM2 DQM3*/
#define FMC_DQM0_PORT               GPIOE
#define FMC_DQM0_PIN                LL_GPIO_PIN_0
#define FMC_DQM1_PORT               GPIOE
#define FMC_DQM1_PIN                LL_GPIO_PIN_1
#define FMC_DQM2_PORT              GPIOI
#define FMC_DQM2_PIN               LL_GPIO_PIN_4
#define FMC_DQM3_PORT              GPIOI
#define FMC_DQM3_PIN               LL_GPIO_PIN_5

/**
*  SDMMC1 GPIO Configuration
PC8      ------> SDMMC1_D0
PC9      ------> SDMMC1_D1
PC10     ------> SDMMC1_D2
PC11     ------> SDMMC1_D3
PC12     ------> SDMMC1_CLK
PD2      ------> SDMMC1_CMD
*/
#define SDMMC1_GPIO_RCC            (LL_AHB4_GRP1_PERIPH_GPIOC | LL_AHB4_GRP1_PERIPH_GPIOD)
#define SDMMC1_D0_PORT             GPIOC
#define SDMMC1_D0_PIN              LL_GPIO_PIN_8
#define SDMMC1_D0_AF               LL_GPIO_AF_12
#define SDMMC1_D1_PORT             GPIOC
#define SDMMC1_D1_PIN              LL_GPIO_PIN_9
#define SDMMC1_D1_AF               LL_GPIO_AF_12
#define SDMMC1_D2_PORT             GPIOC
#define SDMMC1_D2_PIN              LL_GPIO_PIN_10
#define SDMMC1_D2_AF               LL_GPIO_AF_12
#define SDMMC1_D3_PORT             GPIOC
#define SDMMC1_D3_PIN              LL_GPIO_PIN_11
#define SDMMC1_D3_AF               LL_GPIO_AF_12
#define SDMMC1_CLK_PORT            GPIOC
#define SDMMC1_CLK_PIN             LL_GPIO_PIN_12
#define SDMMC1_CLK_AF              LL_GPIO_AF_12
#define SDMMC1_CMD_PORT            GPIOD
#define SDMMC1_CMD_PIN             LL_GPIO_PIN_2
#define SDMMC1_CMD_AF              LL_GPIO_AF_12

/**SDMMC2 GPIO Configuration
 *
PB14                ------> SDMMC2_D0
PB15                ------> SDMMC2_D1
PB3 (JTDO/TRACESWO) ------> SDMMC2_D2
PB4 (NJTRST)        ------> SDMMC2_D3
PD6                 ------> SDMMC2_CLK
PD7                 ------> SDMMC2_CMD
*/
#define SDMMC2_GPIO_RCC            (LL_AHB4_GRP1_PERIPH_GPIOB | LL_AHB4_GRP1_PERIPH_GPIOD)
#define SDMMC2_D0_PORT             GPIOB
#define SDMMC2_D0_PIN              LL_GPIO_PIN_14
#define SDMMC2_D0_AF               LL_GPIO_AF_9
#define SDMMC2_D1_PORT             GPIOB
#define SDMMC2_D1_PIN              LL_GPIO_PIN_15
#define SDMMC2_D1_AF               LL_GPIO_AF_9
#define SDMMC2_D2_PORT             GPIOB
#define SDMMC2_D2_PIN              LL_GPIO_PIN_3
#define SDMMC2_D2_AF               LL_GPIO_AF_9
#define SDMMC2_D3_PORT             GPIOB
#define SDMMC2_D3_PIN              LL_GPIO_PIN_4
#define SDMMC2_D3_AF               LL_GPIO_AF_9
#define SDMMC2_CLK_PORT            GPIOD
#define SDMMC2_CLK_PIN             LL_GPIO_PIN_6
#define SDMMC2_CLK_AF              LL_GPIO_AF_11
#define SDMMC2_CMD_PORT            GPIOD
#define SDMMC2_CMD_PIN             LL_GPIO_PIN_7
#define SDMMC2_CMD_AF              LL_GPIO_AF_11

/**
  * @brief SD-detect signal
  */
#define SD_DETECT_GPIO_RCC         (LL_AHB4_GRP1_PERIPH_GPIOI)
#define SD_DETECT_PORT             GPIOI
#define SD_DETECT_PIN              LL_GPIO_PIN_8

#endif
