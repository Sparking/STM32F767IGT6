#ifndef _TIMER_H_
#define _TIMER_H_

#include "stm32f7xx.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_tim.h"

/*
 * 宏APB1_TIM_FCLK
 * 作用: APB1定时器的输入频率
 */
#ifndef APB1FCLK
#error "APB1 Frequency is unknown, please set macro APB1FCLK."
#else
# ifndef RCC_DKCFGR1_TIMPRE
# error "Register RCC->DKCFG1_TIMPRE is unkown, please set macro RCC_DKCFGR1_TIMPRE."
# else
#  if (RCC_DKCFGR1_TIMPRE == 0U)
#    if (APB1PRE == 1U)
#      define APB1_TIM_FCLK APB1FCLK
#    else
#      define APB1_TIM_FCLK (APB1FCLK << 1U)
#    endif
#  else
#    if (APB1PRE == 1U || APB1PRE == 2U || APB1PRE == 4U)
#      define APB1_TIM_FCLK HCLK
#    else
#      define APB1_TIM_FCLK (APB1FCLK << 2U)
#    endif
#  endif
# endif
#endif

/*
 * 宏APB2_TIM_FCLK
 * 作用: APB2定时器的输入频率
 */
#ifndef APB2FCLK
#error "APB1 Frequency is unknown, please set macro APB1FCLK."
#else
# ifndef RCC_DKCFGR1_TIMPRE
# error "Register RCC->DKCFG1_TIMPRE is unkown, please set macro RCC_DKCFGR1_TIMPRE."
# else
#  if (RCC_DKCFGR1_TIMPRE == 0U)
#    if (APB2PRE == 1U)
#      define APB2_TIM_FCLK APB2FCLK
#    else
#      define APB2_TIM_FCLK (APB2FCLK << 1U)
#    endif
#  else
#    if (APB2PRE == 1U || APB2PRE == 2U || APB2PRE == 4U)
#      define APB2_TIM_FCLK HCLK
#    else
#      define APB2_TIM_FCLK (APB2FCLK << 2U)
#    endif
#  endif
# endif
#endif

#define TIM3_SetCNT(n)  \
        TIM3->CNT = n
#define TIM3_CalibrationDuty(n) {\
        TIM3->CCR2 = n;          \
}

/* TIM3_PWM_Init
 * @freq: 计数器的时钟频率
 * @period: 技术周期
 * @vaild: 高电平的有效计数值
 * 
 */
void TIM3_PWM_Init(unsigned int freq, unsigned int period, unsigned int vaild);

#endif /* _TIMER_H_ */
