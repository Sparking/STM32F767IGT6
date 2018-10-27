#include <string.h>
#include "gpio.h"
#include "usart.h"

/* _USART_RX_BUF当前已接收的字节个数 */
static volatile size_t USART_REC_LEN = 0;
static char _USART_RX_BUF[UART_RECEIVE_BUFF_MAX_SIZE];

static void uart_universary_read_handler(USART_TypeDef *usartx)
{
    char data;

    data = usartx->RDR;
    _USART_RX_BUF[USART_REC_LEN++] = data;
    USART_REC_LEN = (USART_REC_LEN >= UART_RECEIVE_BUFF_MAX_SIZE) ? 0 : USART_REC_LEN;
    usartx->ISR = 0;
}

void USART1_IRQHandler(void)
{
    if (USART1->ISR & 0x00000020) {
        uart_universary_read_handler(USART1);
    }
    USART1->ISR = 0;
}

void USART2_IRQHandler(void)
{
    if (USART2->ISR & 0x00000020) {
        uart_universary_read_handler(USART2);
    }
    USART2->ISR = 0;
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
        GPIO_InitTypeDef GPIO_Initure;
        if (huart->Instance == USART1) {
        /*
         * USART1使用GPIOA的PIN9和PIN10,并设置为上拉
         */
                GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10;
                GPIO_Initure.Mode = GPIO_MODE_AF_PP;
                GPIO_Initure.Pull = GPIO_PULLUP;
                GPIO_Initure.Speed= GPIO_SPEED_FAST;
                GPIO_Initure.Alternate = GPIO_AF7_USART1;
                GPIO_Clk_Enable(GPIOA);
                HAL_GPIO_Init(GPIOA, &GPIO_Initure);
                __HAL_RCC_USART1_CLK_ENABLE();
                HAL_NVIC_EnableIRQ(USART1_IRQn);
                HAL_NVIC_SetPriority(USART1_IRQn, 15, 0);
        } else if (huart->Instance == USART2) {
        /*
         * USART1使用GPIOA的PIN9和PIN10,并设置为上拉
         */
                GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;
                GPIO_Initure.Mode = GPIO_MODE_AF_PP;
                GPIO_Initure.Pull = GPIO_PULLUP;
                GPIO_Initure.Speed= GPIO_SPEED_FAST;
                GPIO_Initure.Alternate = GPIO_AF7_USART1;
                GPIO_Clk_Enable(GPIOA);
                HAL_GPIO_Init(GPIOA, &GPIO_Initure);
                __HAL_RCC_USART2_CLK_ENABLE();
                HAL_NVIC_EnableIRQ(USART2_IRQn);
                HAL_NVIC_SetPriority(USART2_IRQn, 15, 0);
        }
}

void UART_Init(void)
{
        UART_HandleTypeDef UART_Handler = {0};

        UART_Handler.Instance = STD_UART_CONSOLE_0;             /* 标准输入输出重定向设备 */
        UART_Handler.Init.BaudRate = 115200;                    /* 波特率设置为115200 */
        UART_Handler.Init.WordLength = UART_WORDLENGTH_8B;      /* 字长为8位数据格式 */
        UART_Handler.Init.StopBits = UART_STOPBITS_1;           /* 一个停止位 */
        UART_Handler.Init.Parity = UART_PARITY_NONE;            /* 无奇偶校验位 */
        UART_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;      /* 无硬件流控 */
        UART_Handler.Init.Mode = UART_MODE_TX_RX;               /* 收发模式 */
        UART_Handler.gState = HAL_UART_STATE_RESET;             /* 进入回调初始化函数 */
        HAL_UART_Init(&UART_Handler);
        __HAL_UART_ENABLE_IT(&UART_Handler, UART_IT_RXNE);
}

int USART1ReveiveStr(char *str, int len)
{
        while (!USART_REC_LEN) {
            continue;
        }

        STD_UART_CONSOLE_0->CR1 &= 0xFFFFFFDFU; 
        if (len > USART_REC_LEN ) {
            len = ++USART_REC_LEN;
        }
        USART_REC_LEN = 0;
        strncpy(str, _USART_RX_BUF, len);
        STD_UART_CONSOLE_0->CR1 |= 0x00000020U;
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_RESET);
        
        for (int i = 0; i < len; i++) {
            UARTSendChar(STD_UART_CONSOLE_0, str[i]);
        }

        return len;
}

void UARTSendChar(USART_TypeDef *UARTx, char ch)
{
        while (!(UARTx->ISR & 0x40))
                continue;
        UARTx->TDR = ch;
}
