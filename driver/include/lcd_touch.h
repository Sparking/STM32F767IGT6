#ifndef __LCD_TOUCH_CONFIG_H_
#define __LCD_TOUCH_CONFIG_H_

#include "lcd.h"
#include "gt9147.h"

#define TP_PRES_DOWN 0x80  /* 触屏被按下 */
#define TP_CATH_PRES 0x40  /* 有按键按下了 */
#define CT_MAX_TOUCH  5    /* 电容屏支持的点数,固定为5点 */

/* 触摸屏控制器 */
struct __lcd_tp_device_typedef {
	unsigned char (*init)(void); /* 初始化触摸屏控制器 */
	unsigned char (*scan)(void); /* 扫描触摸屏.0,屏幕扫描;1,物理坐标 */
	unsigned int x[CT_MAX_TOUCH]; /* 当前坐标 */
	unsigned int y[CT_MAX_TOUCH]; /* 电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,
                                         触屏的坐标,用x[4],y[4]存储第一次按下时的坐标. */
	unsigned char sta; /* 笔的状态 b7:按下1/松开0; b6:0,没有按键按下;1,有按键按下.
                                       b5:保留 b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下) */
        LCD_DirectionTypeDef dir;
};

extern struct __lcd_tp_device_typedef __lcd_tp_device;

#define LCD_Touch_Init() __lcd_tp_device.init()

#endif
