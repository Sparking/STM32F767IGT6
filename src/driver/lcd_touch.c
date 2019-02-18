#include "lcd_touch.h"

static unsigned char __lcd_TP_Init(void);

struct __lcd_tp_device_typedef __lcd_tp_device = {
	.init = __lcd_TP_Init,
	.scan = NULL,
	.sta = 0,
        .dir = LCD_DIRECTION_V
};

static unsigned char __lcd_TP_Init(void)
{
        GT9147_Init();
        __lcd_tp_device.scan = GT9147_Scan; //扫描函数指向GT9147触摸屏扫描 
        return 0;                
}
