/**
  ******************************************************************************
  * @file    main.c
  * @author  Nassura
  * @version V1.0
  * @date    2017-12-9
  * @brief   Default main function.
  ******************************************************************************
*/

#include "main.h"

#define TIM3_FREQ         1000    /* 定时器的时钟频率设置为1KHz */
#define TIM3_PWM2_PERIOD  500    /* 定时器的计数周期设置位500，即一个周期500ms */
extern void RTC_WKUP_IRQHandler(void);

/* IIC设备接口1 */
IIC_GPIOInterfaceTypeDef IIC_Pin_Interface1;
IIC_DeviceTypeDef _at24c02_dev;
const char *const String = "XXC Test.\r\n";

/*
 * 将字符串全部填为空格符
 */
void  Fill_Str(char *str, int n)
{
        int len = strlen(str);
        while (len < n - 1)
                str[len++] = ' ';
        str[len] = '\0';
}

void LCD_Show(void)
{
        char str[29];
        LCD_CharConfigTypeDef LCD_String;

        /* LCD显示字符串 */
        LCD_String.FontConfig.Font = &ASCII_Font[1]; /* 要显示的字体 */
        /* 字体是否需要其他颜色衬托(是否需要背景色) */
        LCD_String.FontConfig.Mode = FONT_DISPLAY_WITH_BACK_COLOR;
        /* 背景色设置为白色 */
        LCD_String.FontConfig.BackColor = LCD_BG_COLOR;
        LCD_String.PointConfig.Color = 0x0000U; /* 字符串的颜色设置为黑色 */
        LCD_String.PointConfig.Layer = LCD_LAYER_1; /* 在第一层显示 */
        LCD_String.PointConfig.Position.X = 0; /* 字符串开始地址X轴 */
        LCD_String.PointConfig.Position.Y = 0; /* 字符串开始地址Y轴 */
        /* 竖屏显示 */
        LCD_String.PointConfig.Direction = LCD_DIRECTION_V;
        sprintf(str, "Current Temperature: NULL");
        Fill_Str(str, 29);
        LCD_String.PointConfig.Position.X = 0;
        LCD_String.PointConfig.Position.Y = 32;
        LCD_Puts(&LCD_String, str);
        AT24CXX_ReadByte(&_at24c02_dev, 0, (unsigned char *)str);
        sprintf(str, "Target Temperature: %d", (int)str[0]);
        Fill_Str(str, 29);
        LCD_String.PointConfig.Position.Y = 48;
        LCD_Puts(&LCD_String, str);
        AT24CXX_ReadByte(&_at24c02_dev, 1, (unsigned char *)str);
        sprintf(str, "Heating Status:%s", str[0] == 1 ? "heating" : "stopped");
        Fill_Str(str, 29);
        LCD_String.PointConfig.Position.Y = 64;
        LCD_Puts(&LCD_String, str);
}

void EXTI15_10_IRQHandler(void)
{  
}

void main_init(void)
{
        unsigned char i;
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;

        /* IIC 设备初始化 */
        IIC_Pin_Interface1.SCL.GPIO = GPIOH;
        IIC_Pin_Interface1.SCL.Pin  = GPIO_PIN_4;
        IIC_Pin_Interface1.SDA.GPIO = GPIOH;
        IIC_Pin_Interface1.SDA.Pin  = GPIO_PIN_5;
        _at24c02_dev.IIC_Pin_Interface = &IIC_Pin_Interface1;
        _at24c02_dev.IIC_Device_Address = AT24CXX_ADDR;

        HAL_Init();
        SDRAM_Init();
        LCD_Init();
        LCD_Touch_Init();
        LCD_Clear(LCD_BG_COLOR);
        Key_Init();
        LED_Init();
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_1, GPIO_PIN_SET);
        RTC_ConfigTime(&time, 9, 6, 50, 1);
        RTC_ConfigDate(&date, 18, 4, 25, 3);
        RTC_Init(&time, &date);
        RTC_WeakUp_Enable(WKUP_CLK_PRE_16, 2048); /* 1秒1次唤醒中断 */
        UART_Init();
        printf("UART Initialized successed!\r\n");
        IIC_Init(&_at24c02_dev);
        while (!AT24CXX_Check(&_at24c02_dev)) {
                printf("24C02 Check Failed!\r\n");
                printf("Please Check!      \r\n");
                delayms(500);
        }
        printf("24C02 Ready!\r\n");
        TIM3_PWM_Init(TIM3_FREQ, TIM3_PWM2_PERIOD, 0);
        AT24CXX_ReadByte(&_at24c02_dev, 0, &i);
        printf("The last set value is %u.\r\n", i);
        TIM3->CCR2 = 0;

        IWDG_Init();
        RTC_WKUP_IRQHandler();
}

int main(void)
{
    unsigned int temp;
    unsigned char heat_flag;
    unsigned short times;

    main_init();
    times = 0;
    AT24CXX_ReadByte(&_at24c02_dev, 1, &heat_flag);
    while (1) {
        if (times % 200 == 0) {
            IWDG_Feed();
        }
        if (times % 500 == 0) {
            LCD_Show();
        }
        if (times % 50000 == 0) {
            printf("flag is %d.\r\n", heat_flag);
        }
        switch (Key_Scan(KEY_SCAN_MODE_KLI)) {
        case KEY0_PRESSED:
            heat_flag = !heat_flag;
            AT24CXX_WriteByte(&_at24c02_dev, 1, heat_flag);
            printf("Change heat mode!\r\n");
            break;
        case KEY1_PRESSED:
            if (user_interface_readkeyboard(&temp, 8)) {
                IWDG_Feed();
                printf("input %d\r\n", temp);
                if (temp <= 100) {
                    AT24CXX_WriteByte(&_at24c02_dev, 0, (unsigned char)temp);
                }
            }
            break;
        case KEY2_PRESSED:
            printf("pressed key2\r\n");
            break;
        case WK_UP_PRESSED:
            printf("pressed weakup key\r\n");
            break;
        default:
            break;
        }
        heat_flag %= 4;
        delayms(1);
        times++;
    }
}

