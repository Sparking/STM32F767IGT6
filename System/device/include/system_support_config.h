#ifndef __SYSTEM_CLK_CONFIG_H_
#define __SYSTEM_CLK_CONFIG_H_

/*
 * -------------系统时钟参数设置---------------------
 */
#define HSE_USE_CRYSTAL              /* 外部时钟源使用晶振 */
#define HSE_VALUE       25000000UL   /* 外部时钟频率、晶振频率(Hz) */
#define HCLK            216000000UL  /* 主PLL频率(也就是系统的运行频率)(Hz) */

/* 外设时钟源(APB1,APB2)设置 */
#define APB1FCLK        54000000UL   /* APB1时钟频率(最高54MHz) */
#define APB1PRE         4            /* APB1分频系数 */
#define APB2FCLK        108000000UL  /* APB2时钟频率(最高108MHz) */
#define APB2PRE         2            /* APB2分频系数 */

/* PLL时钟系数设置 */
#define PLLM_VALUE      (HSE_VALUE/1000000)/* PLLM 分频得到1MHz的频率 */
#define PLLN_VALUE      ((HCLK/1000000)<<1)/* PLLN 倍频系数 (这里设置为2) */
#define PLLP_VALUE      0U                 /* PLLP 主PLL分频系数 0:2分频 1:4分频 2:6分频 3:8分频 */
#define PLLQ_VALUE      6U                 /* PLLQ PLL48CLK分频系数(分频后的频率最高为48MHz,只能为2-15的整数),这里设置为6(分到36MHz的频率) */

/* PLLSAI时钟配置 */
#define PLLSAI_ENABLE           1U         /* PLLSAI开启判断条件 0:关闭PLLSAI 1:使能PLLSAI */
#if (PLLSAI_ENABLE == 1U)
#define RCC_DKCFGR1_TIMPRE      0U         /* 寄存器RCC_DKCFGR1TIMPRE中的位TIMPRE(其作用是用于给定时器时钟预分频器选择) */
#define PLLSAIN_VALUE           288        /* PLLSAIN(范围:49~432) */
#define PLLSAIP_VALUE           9          /* PLLSAIP(范围:2~15) */
#define PLLSAIQ_VALUE           2          /* PLLSAIQ(范围:2, 4, 6, 8.对应的设置值:0, 1, 2, 3) */
#define PLLSAIR_VALUE           4          /* PLLSAIR(范围:2~7) */
#define PLLSAIDIVQ_VALUE        0          /* PLLSAIDIVQ(范围:1~32.对应的设置值:0~31) */
#define PLLSAIDIVR_VALUE        2          /* PLLSAIDIVR(范围:2, 4, 8, 16.对应的设置值:0, 1, 2, 3) */
#define SAI2SEL_VALUE           0          /* SAI2时钟源选择 使用详情请查看STM32F7xx参考手册 */
#define SAI1SEL_VALUE           0          /* SAI1时钟源选择 */
#endif

/*
 * -------------实时操作系统的支持设置---------------------
 */
#define SYSTEM_SUPPORT_OS       1U         /* 如果值为0，表示不支持操作系统。如果值不为0,则表示支持操作系统 */

#endif
