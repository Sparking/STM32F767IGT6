#include "timer.h"
#include "gpio.h"

/* PWM模式下,频率由TIMx_ARR决定,占空比有TIMx_CCRx寄存器决定
 * OCxM:110,PWM模式1 111,PWM模式2
 * OCxPE置1使能相应的预装载寄存器，最后TIMx_CR1中的ARPW位置1
 * 启动计数器之前，先向TIMx_EGR中的UG位置1
 * 极性通过 TIMx_CCER寄存器中的CCxP位修改
 * OCx输出通过TIMx_CCER寄存器中的CCxE位置1来世能
 */
void TIM3_PWM_Init(unsigned int freq, unsigned int period, unsigned int vaild)
 {
        GPIO_InitTypeDef GPIO_Init;
/* GPIOA PIN7 Channel2 */
        GPIO_Init.Mode = GPIO_MODE_AF_PP;
        GPIO_Init.Speed = GPIO_SPEED_FAST;
        GPIO_Init.Pull = GPIO_PULLDOWN;
        GPIO_Init.Alternate = GPIO_AF2_TIM3;
        GPIO_Init.Pin = GPIO_PIN_7;
        GPIO_Clk_Enable(GPIOA);
        HAL_GPIO_Init(GPIOA, &GPIO_Init);

        __HAL_RCC_TIM3_CLK_ENABLE();
        TIM3->CR1 = 0x00000000U;
/* 以下配置针对通道2 */
        TIM3->CCMR1 &= 0x0010FFFFU;
        TIM3->CCMR1 |= 0x00006000U; /* PWM模式 */
        TIM3->PSC = APB1_TIM_FCLK / freq - 1; /* 分频 */
        TIM3->ARR = period; /* 频率 */
        TIM3->CCR2 = vaild; /* 高电平有效时间 */
        TIM3->CCER |= 0x00000010U; /* 使能通道2的输出使能 */
        TIM3->CR1 |= 0x00000081U;
        TIM3->EGR |= 0x00000001U; /* 更新寄存器 */
}

