/*
 * Copyright (c) 2006-2019, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2019-10-25     xiaoming     first implementation
 */
#include "drv_usart.h"
#include "board.h"
#include <rtdevice.h>
#include <rthw.h>
#include "hw_config.h"

/* STM32 uart driver */
typedef struct 
{
    USART_TypeDef *uart;
    IRQn_Type irq;
} stm32_uart_t;

static void RCC_Configuration(void)
{
#ifdef BSP_USING_UART1
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART1_PORT_RCC);
	/* UART clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
#endif
#ifdef BSP_USING_UART2
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART2_PORT_RCC);
	/* UART clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
#endif
#ifdef BSP_USING_UART3
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART3_PORT_RCC);
	/* UART clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
#endif
#ifdef BSP_USING_UART4
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART4_PORT_RCC);
	/* UART clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART4);
#endif
#ifdef BSP_USING_UART5
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART5_PORT_RCC);
	/* UART clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART5);
#endif
#ifdef BSP_USING_UART6
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART6_PORT_RCC);
	/* UART clock enable */
	LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART6);
#endif
#ifdef BSP_USING_UART7
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART7_PORT_RCC);
	/* UART clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART7);
#endif
#ifdef BSP_USING_UART8
	/* GPIO clock enable */
	LL_AHB4_GRP1_EnableClock(UART8_PORT_RCC);
	/* UART clock enable */
	LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_UART8);
#endif
}
static void GPIO_Configuration(void)
{
    LL_GPIO_InitTypeDef GPIO_InitStruct;
    LL_GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
#ifdef BSP_USING_UART1
    GPIO_InitStruct.Pin = UART1_TX_PIN;
    GPIO_InitStruct.Alternate = UART1_TX_AF;
    LL_GPIO_Init(UART1_TX_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = UART1_RX_PIN;
    GPIO_InitStruct.Alternate = UART1_RX_AF;
    LL_GPIO_Init(UART1_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART2
    GPIO_InitStruct.Pin = UART2_TX_PIN;
    GPIO_InitStruct.Alternate = UART2_TX_AF;
    LL_GPIO_Init(UART2_TX_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = UART2_RX_PIN;
    GPIO_InitStruct.Alternate = UART2_RX_AF;
    LL_GPIO_Init(UART2_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART3
    GPIO_InitStruct.Pin = UART3_TX_PIN;
    GPIO_InitStruct.Alternate = UART3_TX_AF;
    LL_GPIO_Init(UART3_TX_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = UART3_RX_PIN;
    GPIO_InitStruct.Alternate = UART3_RX_AF;
    LL_GPIO_Init(UART3_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART4
    GPIO_InitStruct.Pin = UART4_TX_PIN;
    GPIO_InitStruct.Alternate = UART4_TX_AF;
    LL_GPIO_Init(UART4_TX_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = UART4_RX_PIN;
    GPIO_InitStruct.Alternate = UART4_RX_AF;
    LL_GPIO_Init(UART4_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART5
    GPIO_InitStruct.Pin = UART5_TX_PIN;
    GPIO_InitStruct.Alternate = UART5_TX_AF;
    LL_GPIO_Init(UART5_TX_PORT, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = UART5_RX_PIN;
    GPIO_InitStruct.Alternate = UART5_RX_AF;
    LL_GPIO_Init(UART5_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART6
    GPIO_InitStruct.Pin = UART6_TX_PIN;
    GPIO_InitStruct.Alternate = UART6_TX_AF;
    LL_GPIO_Init(UART6_TX_PORT, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = UART6_RX_PIN;
    GPIO_InitStruct.Alternate = UART6_RX_AF;
    LL_GPIO_Init(UART6_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART7
	GPIO_InitStruct.Pin = UART7_TX_PIN;
    GPIO_InitStruct.Alternate = UART7_TX_AF;
    LL_GPIO_Init(UART7_TX_PORT, &GPIO_InitStruct);
	GPIO_InitStruct.Pin = UART7_RX_PIN;
    GPIO_InitStruct.Alternate = UART7_RX_AF;
    LL_GPIO_Init(UART7_RX_PORT, &GPIO_InitStruct);
#endif
#ifdef BSP_USING_UART8
    GPIO_InitStruct.Pin = UART8_TX_PIN;
    GPIO_InitStruct.Alternate = UART8_TX_AF;
    LL_GPIO_Init(UART8_TX_PORT, &GPIO_InitStruct);
		GPIO_InitStruct.Pin = UART8_RX_PIN;
    GPIO_InitStruct.Alternate = UART8_RX_AF;
    LL_GPIO_Init(UART8_RX_PORT, &GPIO_InitStruct);
#endif

}

static rt_err_t uart_configure(struct rt_serial_device *serial,
                              struct serial_configure *cfg)
{
    stm32_uart_t *dev;
    LL_USART_InitTypeDef USART_InitStruct;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    dev = (stm32_uart_t *)serial->parent.user_data;
    LL_USART_StructInit(&USART_InitStruct);
    USART_InitStruct.BaudRate   = cfg->baud_rate;
    USART_InitStruct.HardwareFlowControl  = LL_USART_HWCONTROL_NONE;
    USART_InitStruct.TransferDirection       = LL_USART_DIRECTION_TX_RX;
    USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
    switch (cfg->data_bits)
    {
    case DATA_BITS_8:
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
        break;
    case DATA_BITS_9:
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_9B;
        break;
    default:
        USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
        break;
    }
    switch (cfg->stop_bits)
    {
    case STOP_BITS_1:
        USART_InitStruct.StopBits   = LL_USART_STOPBITS_1;
        break;
    case STOP_BITS_2:
        USART_InitStruct.StopBits   = LL_USART_STOPBITS_2;
        break;
    default:
        USART_InitStruct.StopBits   = LL_USART_STOPBITS_1;
        break;
    }
    switch (cfg->parity)
    {
    case PARITY_NONE:
        USART_InitStruct.Parity     = LL_USART_PARITY_NONE;
        break;
    case PARITY_ODD:
        USART_InitStruct.Parity     = LL_USART_PARITY_ODD;
        break;
    case PARITY_EVEN:
        USART_InitStruct.Parity     = LL_USART_PARITY_EVEN;
        break;
    default:
        USART_InitStruct.Parity     = LL_USART_PARITY_NONE;
        break;
    }
    if (LL_USART_Init(dev->uart,&USART_InitStruct) != SUCCESS)
    {
        return RT_ERROR;
    }
		LL_USART_Enable(dev->uart);
		
    return RT_EOK;
}

static rt_err_t uart_control(struct rt_serial_device *serial,
                            int cmd, void *arg)
{
    stm32_uart_t *dev;
    RT_ASSERT(serial != RT_NULL);
    dev = (stm32_uart_t *)serial->parent.user_data;
    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        /* disable rx irq */
        NVIC_DisableIRQ(dev->irq);
        /* disable interrupt */
        LL_USART_DisableIT_RXNE_RXFNE(dev->uart);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        /* enable rx irq */
        NVIC_EnableIRQ(dev->irq);
        NVIC_SetPriority(dev->irq, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
        /* enable interrupt */
        LL_USART_EnableIT_RXNE_RXFNE(dev->uart);
        break;
    }
    return RT_EOK;
}

static int uart_putc(struct rt_serial_device *serial, char c)
{
    stm32_uart_t *dev;
    RT_ASSERT(serial != RT_NULL);
    dev = (stm32_uart_t *)serial->parent.user_data;
    while ((dev->uart->ISR & USART_ISR_TXE_TXFNF) != USART_ISR_TXE_TXFNF);
    dev->uart->TDR = c;
    return 1;
}

static int uart_getc(struct rt_serial_device *serial)
{
    int ch;
    stm32_uart_t *dev;
    RT_ASSERT(serial != RT_NULL);
    dev = (stm32_uart_t *)serial->parent.user_data;
    ch = -1;
    if (dev->uart->ISR & USART_ISR_RXNE_RXFNE)
        ch = dev->uart->RDR & 0xff;
    return ch;
}

static const struct rt_uart_ops drv_uart_ops =
{
    uart_configure,
    uart_control,
    uart_putc,
    uart_getc,
};

#if defined(BSP_USING_UART1)
/* UART1 device driver structure */
static stm32_uart_t uart1;
struct rt_serial_device serial1;
void USART1_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart1;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial1, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART1 */
#if defined(BSP_USING_UART2)
/* UART1 device driver structure */
static stm32_uart_t uart2;
struct rt_serial_device serial2;
void USART2_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart2;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial2, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART2 */
#if defined(BSP_USING_UART3)
/* UART1 device driver structure */
static stm32_uart_t uart3;
struct rt_serial_device serial3;
void USART3_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart3;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial3, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART3 */
#if defined(BSP_USING_UART4)
/* UART4 device driver structure */
static stm32_uart_t uart4;
struct rt_serial_device serial4;
void UART4_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart4;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial4, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART4 */

#if defined(BSP_USING_UART5)
/* UART5 device driver structure */
static stm32_uart_t uart5;
struct rt_serial_device serial5;
void UART5_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart5;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial5, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART5 */

#if defined(BSP_USING_UART6)
/* UART6 device driver structure */
static stm32_uart_t uart6;
struct rt_serial_device serial6;
void USART6_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart6;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial6, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART6 */

#if defined(BSP_USING_UART7)
/* UART7 device driver structure */
static stm32_uart_t uart7;
struct rt_serial_device serial7;
void UART7_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart7;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial7, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART7 */
#if defined(BSP_USING_UART8)
/* UART7 device driver structure */
static stm32_uart_t uart8;
struct rt_serial_device serial8;
void UART8_IRQHandler(void)
{
    stm32_uart_t *dev;
    dev = &uart8;
    /* enter interrupt */
    rt_interrupt_enter();
    /* UART in mode Receiver -------------------------------------------------*/
    if ((dev->uart->ISR & USART_ISR_RXNE_RXFNE) &&
            (dev->uart->CR1&USART_CR1_RXNEIE_RXFNEIE))
    {
        rt_hw_serial_isr(&serial8, RT_SERIAL_EVENT_RX_IND);
        /* Clear RXNE interrupt flag */
        dev->uart->ICR = USART_ISR_RXNE_RXFNE;
    }
    /* leave interrupt */
    rt_interrupt_leave();
}
#endif /* BSP_USING_UART8 */
int rt_hw_usart_init(void)
{
    stm32_uart_t *dev;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
	  RCC_Configuration();
		GPIO_Configuration();
#ifdef BSP_USING_UART1
    dev = &uart1;
    dev->uart = USART1;
    dev->irq = USART1_IRQn;
    serial1.ops    = &drv_uart_ops;
    serial1.config = config;
    /* register UART1 device */
    rt_hw_serial_register(&serial1, "uart1",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART1 */
#ifdef BSP_USING_UART2
    dev = &uart2;
    dev->uart = USART2;
    dev->irq = USART2_IRQn;
    serial2.ops    = &drv_uart_ops;
    serial2.config = config;
    /* register UART2 device */
    rt_hw_serial_register(&serial2, "uart2",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART1 */
#ifdef BSP_USING_UART3
    dev = &uart3;
    dev->uart = USART3;
    dev->irq = USART3_IRQn;
    serial3.ops    = &drv_uart_ops;
    serial3.config = config;
    /* register UART3 device */
    rt_hw_serial_register(&serial3, "uart3",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART4 */
#ifdef BSP_USING_UART4
    dev = &uart4;
    dev->uart = UART4;
    dev->irq = UART4_IRQn;
    serial4.ops    = &drv_uart_ops;
    serial4.config = config;
    /* register UART4 device */
    rt_hw_serial_register(&serial4, "uart4",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART4 */
#ifdef BSP_USING_UART5
    dev = &uart5;
    dev->uart = UART5;
    dev->irq = UART5_IRQn;
    serial5.ops    = &drv_uart_ops;
    serial5.config = config;
    /* register UART5 device */
    rt_hw_serial_register(&serial5, "uart5",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART5 */
#ifdef BSP_USING_UART6
    dev = &uart6;
    dev->uart = USART6;
    dev->irq = USART6_IRQn;
    serial6.ops    = &drv_uart_ops;
    serial6.config = config;
    /* register UART6 device */
    rt_hw_serial_register(&serial6, "uart6",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART6 */
#ifdef BSP_USING_UART7
    dev = &uart7;
    dev->uart = UART7;
    dev->irq = UART7_IRQn;
    serial7.ops    = &drv_uart_ops;
    serial7.config = config;
    /* register UART7 device */
    rt_hw_serial_register(&serial7, "uart7",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART7 */
#ifdef BSP_USING_UART8
    dev = &uart8;
    dev->uart = UART8;
    dev->irq = UART8_IRQn;
    serial8.ops    = &drv_uart_ops;
    serial8.config = config;
    /* register UART8 device */
    rt_hw_serial_register(&serial8, "uart8",
                          RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX,
                          dev);
#endif /* BSP_USING_UART8 */
    return 0;
}
INIT_BOARD_EXPORT(rt_hw_usart_init);
