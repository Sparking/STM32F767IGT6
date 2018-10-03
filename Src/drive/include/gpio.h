#ifndef __GPIO_CONFIG_H_
#define __GPIO_CONFIG_H_

#include "stm32f7xx.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_gpio.h"

typedef struct {
        GPIO_TypeDef *GPIO;
        unsigned int  Pin;
} Single_GPIOPinTypeDef;

#define GPIO_ResetPin(GPIOx,Pinx)                      \
        HAL_GPIO_WritePin(GPIOx, Pinx, GPIO_PIN_RESET)
#define GPIO_SetPin(GPIOx,Pinx)                        \
        HAL_GPIO_WritePin(GPIOx, Pinx, GPIO_PIN_SET)
#define GPIO_WritePin(GPIOx,Pinx,Bin) \
        HAL_GPIO_WritePin(GPIOx, Pinx, (GPIO_PinState)(Bin))
#define GPIO_ReadPin(GPIOx,Pinx)                        \
        HAL_GPIO_ReadPin(GPIOx,Pinx)

/*
 * GPIO_EasySwitchPinMode
 * @mode: GPIO_MODE_PIN_INPUT GPIO_MODE_OUT_PP
 *
 * 简单切换GPIO引脚的模式(只切换GPIO输入输出模式)
 */
void GPIO_EasySwitchPinMode(GPIO_TypeDef *GPIO, unsigned int pin,
        unsigned int mode);
/*
 * 使能相应的GPIO时钟
 */
void GPIO_Clk_Enable(GPIO_TypeDef *GPIO);

/* read the Keys' value */
#define WK_UP                   HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0)  /* WK_UP Key */
#define KEY2                    HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) /* PC13 Key */
#define KEY1                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_2)  /* PH2 Key */
#define KEY0                    HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_3)  /* PH3 Key */

/* The Value When the key is pressed */
#define WK_UP_PRESSED_VALUE     1       /* the value WK_UP is pressed */
#define KEY2_PRESSED_VALUE      0       /* the value KEY2 is pressed */
#define KEY1_PRESSED_VALUE      0       /* the value KEY1 is pressed*/
#define KEY0_PRESSED_VALUE      0       /* the value KEY0 is pressed*/

/* Key Status */
#define WK_UP_PRESSED           0x01    /* WK_UP is pressed */
#define KEY2_PRESSED            0x02    /* KEY2 is pressed */
#define KEY1_PRESSED            0x04    /* KEY1 is pressed*/
#define KEY0_PRESSED            0x08    /* KEY0 is pressed*/

/* Key Scan Mode */
#define KEY_SCAN_MODE_KLI       00U     /* 保留上一次按钮值 */  
#define KEY_SCAN_MODE_NKLI      01U     /* 不保留上一次按钮值 */

/* 按键初始化 */
void Key_Init(void);
/* 扫描按键 */
/*
 * 参数scan_mode用于设置按键扫描模式
 * KEY_SCAN_MODE_KEEP    支持连续扫描
 * KEY_SCAN_MODE_NOKEEP  每次扫描都将返回当前已被按下的键值
 *
 * 函数将返回被按下的按键值
 * WK_UP_PRESSED 
 * KEY2_PRESSED
 * KEY1_PRESSED
 * KEY0_PRESSED
 */
uint8_t Key_Scan(uint8_t scan_mode);

/* LED初始化 */
void LED_Init(void);

#endif
