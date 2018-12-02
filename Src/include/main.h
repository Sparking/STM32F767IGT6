#ifndef __MAIN_H_
#define __MAIN_H_

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include "cli.h"

#include "at24cxx.h"
#include "delay.h"
#include "gpio.h"
#include "usart.h"
#include "rtc.h"
#include "number_lib.h"
#include "timer.h"
#include "iwdg.h"

#if (SYSTEM_SUPPORT_OS != 0U)
#include "FreeRTOS.h"
#include "task.h"
#endif

#include "user_gui.h"
#include "user_lcd_touch.h"
#include "user_setting_interface.h"
#include "user_pid.h"

#endif
