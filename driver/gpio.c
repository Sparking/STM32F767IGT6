#include "gpio.h"

void GPIO_EasySwitchPinMode(GPIO_TypeDef *GPIO, unsigned int pin,
        unsigned int mode)
{
    unsigned int mode_pos;

    mode_pos = POSITION_VAL(pin) << 1U;
    GPIO->MODER &= ~(3U << mode_pos);
    if (mode) {
        GPIO->MODER |= 1U << mode_pos;
    }
}

void GPIO_Clk_Enable(GPIO_TypeDef *GPIO)
{
    RCC->AHB1ENR |= 1 << (((uint32_t)GPIO - AHB1PERIPH_BASE) >> 10);
}

void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    /* LED Configure */
    GPIO_Initure.Pin  = GPIO_PIN_0 | GPIO_PIN_1;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_HIGH;
    GPIO_Clk_Enable(GPIOB);
    HAL_GPIO_Init(GPIOB, &GPIO_Initure);
}

void Key_Init(void)
{
    GPIO_InitTypeDef GPIO_Initure;

    /* Keyboard Configure */
    /* WK_UP Key*/
    GPIO_Initure.Pin = GPIO_PIN_0;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLDOWN;
    GPIO_Initure.Speed= GPIO_SPEED_LOW;
    GPIO_Clk_Enable(GPIOA);
    HAL_GPIO_Init(GPIOA, &GPIO_Initure);
    /* PC13 Key */
    GPIO_Initure.Pin = GPIO_PIN_13;
    GPIO_Initure.Mode = GPIO_MODE_INPUT;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_LOW;
    GPIO_Clk_Enable(GPIOC);
    HAL_GPIO_Init(GPIOC, &GPIO_Initure);
    /* PH2, PH3 Key */
    GPIO_Initure.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_Clk_Enable(GPIOH);
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
}

uint8_t Key_Scan(uint8_t scan_mode)
{
    uint8_t ret = 0;
    static char flag = 0;

    if (scan_mode != KEY_SCAN_MODE_KLI) {
        flag = 1;
    }

    if (flag && (WK_UP == WK_UP_PRESSED_VALUE || KEY0 == KEY0_PRESSED_VALUE ||
        KEY1 == KEY1_PRESSED_VALUE || KEY2 == KEY2_PRESSED_VALUE)) {
        HAL_Delay(20);
        if (WK_UP == WK_UP_PRESSED_VALUE)
            ret |= WK_UP_PRESSED;
        if (KEY0 == KEY0_PRESSED_VALUE)
            ret |= KEY0_PRESSED;
        if (KEY1 == KEY0_PRESSED_VALUE)
            ret |= KEY1_PRESSED;
        if (KEY2 == KEY0_PRESSED_VALUE)
            ret |= KEY2_PRESSED;
        if (ret)
            flag = 0;
    } else if (WK_UP != WK_UP_PRESSED_VALUE && KEY0 != KEY0_PRESSED_VALUE &&
        KEY1 != KEY1_PRESSED_VALUE && KEY2 != KEY2_PRESSED_VALUE) {
            flag = 1;
    }

    return ret;
}
