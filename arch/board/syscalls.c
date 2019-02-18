#include <stdio.h>
#include "usart.h"

#if defined ( __CC_ARM )

#pragma import(__use_no_semihosting)

struct __FILE {
        int handler;
};

FILE __stdout;

void _sys_exit(int x)
{
        x = x;
}

int fputc(int ch, FILE *f)
{
        UARTSendChar(STD_UART_CONSOLE_0, ch);
        return ch;
}

#elif defined ( __GNUC__ )

/******************************************************************************
 **************************    Retarget Functions *****************************
 ******************************************************************************
 */
/* Write one char "ch" to the default console
 * Need implementing with UART here. */
__attribute__((weak)) void _ttywrch(int ch)
{
        UARTSendChar(STD_UART_CONSOLE_0, ch);
}

/* Initialize UART */
__attribute__((weak)) void retarget_init() {}

/*
* Read "len" of char to "ptr" from file id "fd"
* Return number of char read.
* Need implementing with UART here.
*/
__attribute__((weak)) int _read (int fd, char *ptr, int len)
{
        return len;
}

/* Write "len" of char from "ptr" to file id "fd"
 * Return number of char written.
 * Need implementing with UART here. */
int _write(int fd, char *ptr, int len)
{
        int i = 0;
        USART_TypeDef *UARTx;
        switch (fd) {
                case 0:
                        UARTx = STD_UART_CONSOLE_0;
                        break;
                case 1:
                        UARTx = STD_UART_CONSOLE_1;
                        break;
                default:
                        UARTx = STD_UART_CONSOLE_2;
                        break;
        }
        while( *ptr && i < len) {
                UARTSendChar(UARTx, *ptr);
                ++ i;
                ++ ptr;
        }
        return len;
}

#else

#error "Unknown Complier"

#endif
