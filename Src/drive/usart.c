#include "gpio.h"
#include "usart.h"

static unsigned char *USART_RX_BUF = NULL; /* 接收缓冲,最大USART_REC_LEN个字节 */
static size_t USART_REC_LEN = UART_RECEIVE_BUFF_MAX_SIZE;
static unsigned char _USART_RX_BUF[UART_RECEIVE_BUFF_MAX_SIZE];
/*
 * 接收状态
 * bit15	接收完成标志
 * bit14	接收到0x0D
 * bit13~0	接收到的有效字节数目
 */
static unsigned int USART_RX_STA = 0U;

static void uart_universary_handler(USART_TypeDef *usartx)
{
    unsigned char data;

    data = usartx->RDR;
	if ((USART_RX_STA & 0x8000) == 0) { /* 接收未完成 */
        if (USART_RX_BUF == NULL) {
            USART_RX_BUF = _USART_RX_BUF;
            USART_REC_LEN = sizeof(_USART_RX_BUF);
        }
	    if (USART_RX_STA & 0x4000) {    /* 接收到了0x0D */
		    if (data != 0x0A) {
                USART_RX_STA = 0;       /* 接收错误,重新开始 */
            } else {
                USART_RX_STA |= 0x8000;	/* 接收完成了 */
            }
        } else {                        /* 还没收到0X0D */
            if (data == 0x0D) {
                 USART_RX_STA |= 0x4000;
            } else {
                USART_RX_BUF[USART_RX_STA & 0X3FFF] = data;
                USART_RX_STA ++;
                if ((USART_RX_STA & 0x3FFF) > (USART_REC_LEN - 1)) {
                    USART_RX_STA = 0;   /* 接收数据错误,重新开始接收 */
                }
            }
        }
    }
}

void USART1_IRQHandler(void)
{
    if (USART1->ISR & 0x00000020) {
        uart_universary_handler(USART1);
    }
}

void USART2_IRQHandler(void)
{
    if (USART2->ISR & 0x00000020) {
        uart_universary_handler(USART2);
    }
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
    	UART_Handler.Init.Parity = UART_PARITY_NONE;		/* 无奇偶校验位 */
    	UART_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;      /* 无硬件流控 */
    	UART_Handler.Init.Mode = UART_MODE_TX_RX;		/* 收发模式 */
        UART_Handler.gState = HAL_UART_STATE_RESET;             /* 进入回调初始化函数 */
        HAL_UART_Init(&UART_Handler);
        __HAL_UART_ENABLE_IT(&UART_Handler, UART_IT_RXNE);
}

int USART1ReveiveStr(char *str, int len)
{
        uint16_t count;
        USART_RX_BUF = (unsigned char *)str;
        USART_REC_LEN = len - 1;
        while ((USART_RX_STA & 0x8000) == 0)
                continue;
        count = USART_RX_STA & 0x2FFF;
        USART_RX_STA = 0;
        USART_RX_BUF = NULL;
        str[count] = '\0';
        return count;
}

void UARTSendChar(USART_TypeDef *UARTx, char ch)
{
        while (!(UARTx->ISR & 0x40))
                continue;
        UARTx->TDR = ch;
}
