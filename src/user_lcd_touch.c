#include "user_lcd_touch.h"

unsigned char LCDTouch_Get_Points(LCD_DirectionTypeDef lcd_dir)
{
        unsigned char points = 0;

        __lcd_tp_device.sta = 0;
        __lcd_tp_device.dir = lcd_dir;
        __lcd_tp_device.scan();
        if(__lcd_tp_device.sta & 0xC0) {
                points = __lcd_tp_device.sta & 0x1F;
                points = POSITION_VAL(~points);
        }

        return points;
}

