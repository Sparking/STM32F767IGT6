#ifndef _USART_CONFIG_H_
#define _USART_CONFIG_H_
#include "stm32f7xx.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_dma.h"
#include "stm32f7xx_hal_usart.h"
#include "stm32f7xx_hal_uart.h"

/* 重定向外设 */
#define STD_UART_CONSOLE_0      USART2
#define STD_UART_CONSOLE_1      USART2
#define STD_UART_CONSOLE_2      USART2

#define UART_RECEIVE_BUFF_MAX_SIZE      1024

extern void UART_Init(void);
extern void UARTSendChar(USART_TypeDef *UARTx, char ch);
extern int  USART1ReveiveStr(char *str, int len);

#endif
