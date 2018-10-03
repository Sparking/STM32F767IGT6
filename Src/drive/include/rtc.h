#ifndef __RTC_CONFIG_H_
#define __RTC_CONFIG_H_

#include "stm32f7xx.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_rtc.h"

#include "number_lib.h"

#define RTC_USE_APM

/* 备份域寄存器内容设定 */
#define BKP_RISTER_0_VALUE      0xE5000C10U

#define RTC_Weekday2Str(weekday)  Weekday[weekday - 1]

extern char *Weekday[];

/* RTC返回值,可通过与运算来判断RTC初始化状况 */
#define RTC_INIT_SUCCESS                0x00000000U /* RTC初始化成功 */
#define RTC_CALENDAR_INIT_FAILURE       0x00000001U /* 日历初始化失败 */
#define RTC_CALENDAR_SYNC_FAILURE       0x00000002U /* 日历同步失败 */
unsigned int RTC_Init(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date);
unsigned int RTC_Update(RTC_TimeTypeDef *time, RTC_DateTypeDef *date);

void RTC_ConfigTime(RTC_TimeTypeDef *time, unsigned char hour,
        unsigned char minute, unsigned char second, unsigned char apm);
void RTC_ConfigDate(RTC_DateTypeDef *date, unsigned int year,
        unsigned char month, unsigned char day, unsigned char week);
void RTC_ReadTime(RTC_TimeTypeDef *rtc_time);
void RTC_ReadDate(RTC_DateTypeDef *rtc_date);

#define WKUP_CLK_PRE_16 0 /* RTC/16 时钟 */
#define WKUP_CLK_PRE_8  1 /* RTC/8 时钟 */
#define WKUP_CLK_PRE_4  2 /* RTC/4 时钟 */
#define WKUP_CLK_PRE_2  3 /* RTC/2 时钟 */
#define WKUP_CLK_SPRE_1 4 /* ck_spre(通常位1Hz) */
#define WKUP_CLK_SPRE_2 6 /* ck_spre(通常位1Hz), WUT计数值增加到0x00010000U */
void RTC_WeakUp_Enable(unsigned int clk_src, unsigned int wkup_cnt);

#endif
