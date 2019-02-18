#ifndef _USER_LCD_TOUCH_H_
#define _USER_LCD_TOUCH_H_

#include "lcd_touch.h"

unsigned char LCDTouch_Get_Points(LCD_DirectionTypeDef lcd_dir);

/* 获取第n个触摸点的位置 */
#define LCDTouch_Get_PointX(pointn) __lcd_tp_device.x[pointn]
#define LCDTouch_Get_PointY(pointn) __lcd_tp_device.y[pointn]

#endif /* End of _USER_LCD_TOUCH_H_ */
