/**
  ******************************************************************************
  * @file    stm32f7xx_it.c
  * @author  Ac6
  * @version V1.0
  * @date    02-Feb-2015
  * @brief   Default Interrupt Service Routines.
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#ifdef USE_RTOS_SYSTICK
#include <cmsis_os.h>
#endif
#include "stm32f7xx_it.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            	  	    Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles SysTick Handler, but only if no RTOS defines it.
  * @param  None
  * @retval None
  */

void SysTick_Handler(void)
{
#if (SYSTEM_SUPPORT_OS == 0U)
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();

#ifdef USE_RTOS_SYSTICK
	osSystickHandler();
#endif

#else
        /* 系统已经开始运行 */
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
                xPortSysTickHandler();
        }
        HAL_IncTick();
#endif
}

void RTC_WKUP_IRQHandler(void)
{
        char str[32];
        RTC_DateTypeDef date;
        RTC_TimeTypeDef time;
        LCD_CharConfigTypeDef LCD_String;

        RTC->ISR &= 0xFFFFFBFFU; /* 清除WUTF唤醒标志位 */
        EXTI->PR |= 0x00400000U;
        RTC_ReadDate(&date);
        /* LCD显示字符串 */
        sprintf(str, "%-23s", "Date:");
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

        LCD_Puts(&LCD_String, str);
        sprintf(str, "%-23s", "Time:");
        LCD_String.PointConfig.Position.X = 0;
        LCD_String.PointConfig.Position.Y = 16;
        LCD_Puts(&LCD_String, str);
        sprintf(str, "20%02u-%02u-%02u %-12s", date.Year, date.Month,
                date.Date, RTC_Weekday2Str(date.WeekDay));
        LCD_String.PointConfig.Position.X = 40;
        LCD_String.PointConfig.Position.Y = 0;
        LCD_Puts(&LCD_String, str);
        RTC_ReadTime(&time);
        sprintf(str, "%02u:%02u:%02u %cM", time.Hours, time.Minutes,
                time.Seconds, time.TimeFormat == RTC_HOURFORMAT12_AM
                ? 'A' : 'P');
        LCD_String.PointConfig.Position.X = 40;
        LCD_String.PointConfig.Position.Y = 16;
        LCD_Puts(&LCD_String, str);
}
