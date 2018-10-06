#include "rtc.h"
/* RTC选择LSE作为时钟源时的两个分频系数 */
/* LSE的外部时钟晶振频率为32.768kHz */
/* 要保证分频后的频率为1Hz,计算公式为
 *     F(rtc_clk) = F(LSE_clk) / ((RTC_PREDIV_A+1)*(RTC_PREDIV_s+1))
 */
#define RTC_PREDIV_A  0x7F /* 异步分频系数,最大为125,越大越好 */
#define RTC_PREDIV_S  0xFF /* 同步分频系数 */

char *Weekday[] = {"Monday", "Tuesday", "Wednesday", "Thursday",
                "Friday", "Saturday", "Sunday"};

unsigned int RTC_Init(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date)
{
        volatile uint32_t *bkp_reg_base_addr = &RTC->BKP0R;
        unsigned int ret = RTC_INIT_SUCCESS;

        /* 电源时钟使能 */
        __HAL_RCC_PWR_CLK_ENABLE();
        /* 使能对RTC、RTC备份寄存器和备份SRAM的访问 */
        PWR->CR1 |= 0x00000100U;
        /* 使能RTC,设置RTC时钟源为LSE,高性能驱动,开启LSE */
        RCC->BDCR = 0x00008119U;
        while ((RCC->BDCR & 0x00000002U) == 0)
                continue; /* 等待LSE稳定 */
        /* 写入RTC寄存器的写保护解锁序列 */
        RTC->WPR  = 0x000000CAU;
        RTC->WPR  = 0x00000053U;
        /* 进入RTC初始化模式 */
        RTC->ISR  = 0x00000080U;
        /* 等待INITF被置为1,即等待进入初始化模式 */
        while ((RTC->ISR & 0x00000040U) == 0)
                continue;
        if (bkp_reg_base_addr[0] != BKP_RISTER_0_VALUE) {
                /* 设置分频系数 */
                RTC->PRER = (RTC_PREDIV_A << 16) + RTC_PREDIV_S;
                /* 时间设置 */
                RTC->TR = NUM2BCD(rtc_time->Seconds)
                        + (NUM2BCD(rtc_time->Minutes) << 8)
                        + ((NUM2BCD(rtc_time->Hours)
                        + rtc_time->TimeFormat) << 16);
                /* 日期设置 */
                RTC->DR = NUM2BCD(rtc_date->Date)
                        + (NUM2BCD(rtc_date->Month) << 8)
                        + (NUM2BCD(rtc_date->Year) << 16)
                        + (rtc_date->WeekDay << 13);
                /* 设置为AM/PM模式,当位6设为0时为24小时格式*/
                RTC->CR = 0x00000040U;
                bkp_reg_base_addr[0] = BKP_RISTER_0_VALUE;
        }
        /* 退出RTC初始化设置 */
        RTC->ISR = 0x0U;
        if ((RTC->ISR & 0x00000010U) == 0)
                ret |= RTC_CALENDAR_INIT_FAILURE;
        else if ((RTC->ISR & 0x00000020U) == 0)
                ret |= RTC_CALENDAR_SYNC_FAILURE;
        /* 写入非解锁数值以再启用写保护 */
        RTC->WPR = 0x000000FFU;

        return ret;
}

void RTC_ConfigDate(RTC_DateTypeDef *date, unsigned int year,
        unsigned char month, unsigned char day, unsigned char week)
{
        date->Year = year;
        date->Month = month;
        date->Date = day;
        date->WeekDay = week;
}

void RTC_ConfigTime(RTC_TimeTypeDef *time, unsigned char hour,
        unsigned char minute, unsigned char second, unsigned char apm)
{
        time->Hours = hour;
        time->Minutes = minute;
        time->Seconds = second;
        time->TimeFormat = apm ? RTC_HOURFORMAT12_PM : RTC_HOURFORMAT12_PM;
}

unsigned int RTC_Update(RTC_TimeTypeDef *rtc_time, RTC_DateTypeDef *rtc_date)
{
        unsigned int ret = 0;

        /* 写入RTC寄存器的写保护解锁序列 */
        RTC->WPR  = 0x000000CAU;
        RTC->WPR  = 0x00000053U;
        /* 进入RTC初始化模式 */
        RTC->ISR  = 0x00000080U;
        /* 等待INITF被置为1,即等待进入初始化模式 */
        while ((RTC->ISR & 0x00000040U) == 0)
                continue;
        /* 时间设置 */
        RTC->TR = NUM2BCD(rtc_time->Seconds) + (NUM2BCD(rtc_time->Minutes) << 8)
                + ((NUM2BCD(rtc_time->Hours) + rtc_time->TimeFormat) << 16);
        /* 日期设置 */
        RTC->DR = NUM2BCD(rtc_date->Date) + (NUM2BCD(rtc_date->Month) << 8)
                + (NUM2BCD(rtc_date->Year) << 16) + (rtc_date->WeekDay << 13);
        /* 退出RTC初始化设置 */
        RTC->ISR = 0x0U;
        if ((RTC->ISR & 0x00000010U) == 0)
                ret |= RTC_CALENDAR_INIT_FAILURE;
        else if ((RTC->ISR & 0x00000020U) == 0)
                ret |= RTC_CALENDAR_SYNC_FAILURE;
        /* 写入非解锁数值以再启用写保护 */
        RTC->WPR = 0x000000FFU;

        return ret;
}

void RTC_ReadTime(RTC_TimeTypeDef *rtc_time)
{
        unsigned int time_src = RTC->TR;

        rtc_time->Hours   = BCD2NUM((time_src >> 16) & 0x3F) % 12;
        rtc_time->Minutes = BCD2NUM((time_src >> 8) & 0x7F);
        rtc_time->Seconds = BCD2NUM(time_src & 0x7F);
        rtc_time->TimeFormat = (time_src >> 16) & RTC_HOURFORMAT12_PM;
}

void RTC_ReadDate(RTC_DateTypeDef *rtc_date)
{
        unsigned int date_src = RTC->DR;

        rtc_date->Year  = (uint8_t)BCD2NUM((date_src >> 16) & 0xFFU);
        rtc_date->Month = (uint8_t)BCD2NUM((date_src >> 8) & 0x1FU);
        rtc_date->Date  = (uint8_t)BCD2NUM(date_src & 0x3FU);
        rtc_date->WeekDay = (date_src >> 13) & 0x7U;
}

/* 失败返回0,成功返回非0 */
unsigned char RTC_Wait_Sync(void)
{
        unsigned int ret;

        RTC->WPR = 0x000000CAU;
        RTC->WPR = 0x00000053U;
        RTC->ISR &= 0xFFFFFFDFU; /* 清除RSF同步标志位 */
        ret = 0x00020000U;
        while (ret-- && (RTC->ISR & 0x00000020U) == 0)
                continue;
        RTC->WPR = 0x000000FFU;

        return ret;
}

void RTC_WeakUp_Enable(unsigned int clk_src, unsigned int wkup_cnt)
{
        if (!RTC_Wait_Sync())
                return;
        RTC->WPR = 0x000000CAU;
        RTC->WPR = 0x00000053U;
        RTC->CR &= 0xFFFFFBFFU; /* 关闭WeakUP */
        while ((RTC->ISR & 0x00000004U) == 0U)
                continue; /* 等待允许更新唤醒定时器配置 */
        RTC->CR &= 0xFFFFFFF8U; /* 清除之前的唤醒时钟选择 */
        RTC->CR |= clk_src; /* 选择时钟源 */
        RTC->WUTR = wkup_cnt; /* 自动重装载值 */
        RTC->ISR &= 0xFFFFFBFFU; /* 清除WUTF唤醒标志位 */
        RTC->CR |= 0x00004400U; /* 开启WKUP和WKUP中断 */
        RTC->WPR = 0x000000FFU; /* 开启写保护 */
        EXTI->PR = 0x00400000U; /* 清除EXTI Line22(对应WKUP中断事件) */
        EXTI->IMR |= 0x00400000U; /* 开启EXTI Line22中断 */
        EXTI->RTSR|= 0x00400000U; /* EXTI Line22事件设置为上升沿触发 *、 */
        HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
        HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 15, 0);
}
