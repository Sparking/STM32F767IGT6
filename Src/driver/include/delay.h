#ifndef __DELAY_CONFIG_H_
#define __DELAY_CONFIG_H_

#include "stm32f7xx.h"
#include "stm32f7xx_hal_cortex.h"

#if (SYSTEM_SUPPORT_OS != 0U)

#include "FreeRTOS.h"
#include "task.h"
/*
 * 函数: delayxms
 * 参数:
 *  delay_time: 延时的毫秒数
 * 返回值: 无
 * 作用: 以毫秒为单位进行延时。延时期间可切换至其他任务。
 */
extern void delayxms(uint32_t delay_time);

#else

#define delayxms(x) delayms(x)

#endif


/*
 * 函数: delayus
 * 参数:
 *  delay_time: 延时的微秒数
 * 返回值: 无
 * 作用: 以微秒为单位进行延时。
 *   如果有操作系统支持，延时期间不可切换至其他任务。
 */
extern void delayus(uint16_t delay_time);


/*
 * 函数: delayms
 * 参数:
 *  delay_time: 延时的毫秒数
 * 返回值: 无
 * 作用: 以毫秒为单位进行延时。
 *   如果有操作系统支持，延时期间不可切换至其他任务。
 */
extern void delayms(uint32_t delay_time);

#endif
