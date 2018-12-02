#include <string.h>

#include "number_lib.h"
#include "rtc.h"
#include "user_setting_interface.h"

/* the decmial number width */
#define DNUM_WIDTH      10

unsigned char user_interface_readkeyboard(unsigned int *key_v,
                unsigned char width)
{
        char buff[DNUM_WIDTH + 1];
        unsigned char end;
        unsigned char i, j;
        unsigned int value;
        LCD_PixelTypeDef text_color;

        j = i = 0;
        text_color = 0xFDFFU;
        GUI_Draw_DigitalKeyboard(80, 40, 7, 5, 0xFFFFU, text_color);
        if (width > DNUM_WIDTH)
                width = DNUM_WIDTH;
        end = width++;
        buff[0] = '\0';
        while (i < width) {
                IWDG_Feed();
                while ((value = GUI_Read_KeyboardValueSteady()) == 0xFF)
                        IWDG_Feed();
                if (value == 11) {
                        text_color = 0x1000U;
                        if (i != 0)
                                i = width;
                }
                else if (value == 9) {
                        if (i != 0)
                                buff[--i] = '\0';
                }
                else {
                        if (value == 10)
                                value = 9;
                        if (i < end) {
                                buff[i++] = (value + 1) % 10 + '0';
                                buff[i] = '\0';
                        }
                }
                value = Str2Num(buff);
                GUI_Draw_ButtonWithText(buff, &ASCII_Font[1], 
                        136 - 4 * 10, 300 - 20, 136 + 4 * 10, 300,
                        0, 0,
                        LCD_DIRECTION_V, LCD_BG_COLOR, text_color,
                        LCD_LAYER_1);
                if (i % width == 0) {
                        j = i;
                        i = 5;
                        while (i-- > 0) {
                                IWDG_Feed();
                                delayms(100);
                        }
                        break;
                }
        }
        if (j)
                *key_v = value;
        GUI_Draw_Window(136 - 4 * 10, 300 - 20, 136 + 4 * 10, 300,
                LCD_DIRECTION_V, LCD_BG_COLOR, LCD_LAYER_1);
        GUI_Draw_DigitalKeyboard(80, 40, 7, 5, LCD_BG_COLOR, LCD_BG_COLOR);

        return j;
}
