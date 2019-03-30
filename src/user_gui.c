#include <stdio.h>
#include <string.h>

#include "delay.h"
#include "user_gui.h"

LCD_WindowTypeDef GUI_Keyboard_KeyWin[12];
char *const GUI_Keyboard_KeyValue[12] = {
        "1", "2", "3", "4", "5", "6", "7", "8", "9", "Del/Cancel", "0", "Set"};

void GUI_Draw_Window(unsigned int start_pos_x, unsigned int start_pos_y,
        unsigned int end_pos_x, unsigned int end_pos_y,
        LCD_DirectionTypeDef lcd_dir, LCD_PixelTypeDef win_color,
        LCD_LayerTypeDef lcd_layer)
{
        LCD_WindowTypeDef lcd_window;
        lcd_window.StartPosition.X = start_pos_x;
        lcd_window.StartPosition.Y = start_pos_y;
        lcd_window.EndPosition.X = end_pos_x;
        lcd_window.EndPosition.Y = end_pos_y;
        lcd_window.Direction = lcd_dir;
        lcd_window.Layer = lcd_layer;
        LCD_Fill_Window(&lcd_window, win_color);
}

void GUI_Draw_ButtonWithText(char *string, const Font_TypeDef *font,
        unsigned int start_pos_x, unsigned int start_pos_y,
        unsigned int end_pos_x, unsigned int end_pos_y,
        unsigned int string_offset_x, unsigned int string_offset_y,
        LCD_DirectionTypeDef lcd_dir, LCD_PixelTypeDef win_color,
        LCD_PixelTypeDef font_color, LCD_LayerTypeDef lcd_layer)
{
        LCD_CharConfigTypeDef LCD_Display;
        GUI_Draw_Window(start_pos_x, start_pos_y, end_pos_x, end_pos_y, lcd_dir,
                win_color, lcd_layer);
        LCD_Display.PointConfig.Color = font_color; /* 字符串的颜色 */
        LCD_Display.PointConfig.Layer = lcd_layer;
        LCD_Display.PointConfig.Direction = lcd_dir;
        /* 字符串开始地址X轴 */
        LCD_Display.PointConfig.Position.X = start_pos_x + string_offset_x;
        /* 字符串开始地址Y轴 */
        LCD_Display.PointConfig.Position.Y = start_pos_y + string_offset_y;
        LCD_Display.FontConfig.Font = font; /* 要显示的字体 */
        /* 字体是否需要其他颜色衬托(是否需要背景色) */
        LCD_Display.FontConfig.Mode = FONT_DISPLAY_WITH_BACK_COLOR;
        LCD_Display.FontConfig.BackColor = win_color;
        LCD_Flush();
        LCD_Puts(&LCD_Display, string);
}

/* 数字键盘 */
void GUI_Draw_DigitalKeyboard(unsigned int key_width, unsigned int key_heigh,
        unsigned int key_x_offset, unsigned int key_y_offset,
        LCD_PixelTypeDef Key_bgcolor, LCD_PixelTypeDef key_color)
{
        unsigned int column;
        unsigned int row;
        unsigned int width;
        unsigned int heigh;
        unsigned int index;
        LCD_CharConfigTypeDef lcd_string;
        LCD_WindowTypeDef window;

        lcd_string.FontConfig.Mode = FONT_DISPLAY_WITH_BACK_COLOR;
        lcd_string.FontConfig.BackColor = key_color; 
        lcd_string.PointConfig.Color = Key_bgcolor;
        lcd_string.PointConfig.Layer = LCD_LAYER_1;
        lcd_string.FontConfig.Font = &ASCII_Font[1];
        lcd_string.PointConfig.Direction = LCD_DIRECTION_V;
        /* 键盘位置设置 */
        width = __ltdc_lcd_device.active_heigh; /* 竖屏显示对应的宽 */
        heigh = __ltdc_lcd_device.active_width; /* 竖屏显示对应的高 */
        window.Layer = LCD_LAYER_1;
        window.Direction = LCD_DIRECTION_V;
        window.EndPosition.X = width;
        window.EndPosition.Y = heigh;
        window.StartPosition.X = 0; 
        window.StartPosition.Y = heigh - 4 * (key_y_offset + key_heigh);
        LCD_Fill_Window(&window, Key_bgcolor);

        /* 画键盘的格子 */
        for (row = 0; row < 4; row++) {
                for (column = 0; column < 3; column++) {
                        /* 竖屏显示 */
                        index = row * 3 + column;
                        GUI_Keyboard_KeyWin[index].Layer = LCD_LAYER_1;
                        GUI_Keyboard_KeyWin[index].Direction =
                                LCD_DIRECTION_V;
                        GUI_Keyboard_KeyWin[index].StartPosition.X =
                                (column + 1) * key_x_offset
                                + column * key_width;
                        GUI_Keyboard_KeyWin[index].StartPosition.Y = heigh
                                - (4 - row - 1) * key_y_offset
                                - (4 - row) * key_heigh;
                        GUI_Keyboard_KeyWin[index].EndPosition.X =
                                GUI_Keyboard_KeyWin[index].StartPosition.X
                                + key_width;
                        GUI_Keyboard_KeyWin[index].EndPosition.Y =
                                GUI_Keyboard_KeyWin[index].StartPosition.Y
                                + key_heigh;
                        LCD_Fill_Window(&GUI_Keyboard_KeyWin[index], key_color);
                        /* 字符串开始地址X轴 */
                        lcd_string.PointConfig.Position.X =
                                GUI_Keyboard_KeyWin[index].StartPosition.X
                                + (key_width - strlen(GUI_Keyboard_KeyValue[index])
                                * lcd_string.FontConfig.Font->Width / 2) / 2;
                        /* 字符串开始地址Y轴 */
                        lcd_string.PointConfig.Position.Y =
                                GUI_Keyboard_KeyWin[index].StartPosition.Y
                                + (key_heigh
                                - lcd_string.FontConfig.Font->Heigh) / 2;
                        LCD_Flush();
                        LCD_Puts(&lcd_string, GUI_Keyboard_KeyValue[index]);
                }
        }
}

/* GUI_Is_TouchInWindow(&key[i]); */
unsigned char GUI_Is_TouchInWindow(LCD_WindowTypeDef *win)
{
        unsigned char ret = 0;
        unsigned char points = LCDTouch_Get_Points(win->Direction);

        if (points != 0) {
                if (LCDTouch_Get_PointX(0) >= win->StartPosition.X
                    && LCDTouch_Get_PointX(0) < win->EndPosition.X
                    && LCDTouch_Get_PointY(0) >= win->StartPosition.Y
                    && LCDTouch_Get_PointY(0) < win->EndPosition.Y) {
                        ret = 1;
                }
        }
        return ret;
}

unsigned char GUI_Read_KeyboardValue(void)
{
        unsigned char i;
        unsigned char points = LCDTouch_Get_Points(LCD_DIRECTION_V);

        i = 0xFF;
        if (points != 0) {
                for (i = 0; i < 12; i++) {
                        if (LCDTouch_Get_PointX(0)
                            >= GUI_Keyboard_KeyWin[i].StartPosition.X
                            && LCDTouch_Get_PointX(0)
                            < GUI_Keyboard_KeyWin[i].EndPosition.X
                            && LCDTouch_Get_PointY(0)
                            >= GUI_Keyboard_KeyWin[i].StartPosition.Y
                            && LCDTouch_Get_PointY(0)
                            < GUI_Keyboard_KeyWin[i].EndPosition.Y) {
                                break;
                        }
                }
                if (i == 12)
                        i = 0xFF;
        }
        return i;
}

unsigned char GUI_Read_KeyboardValueSteady(void)
{
        unsigned char  key = GUI_Read_KeyboardValue();
        /* 消抖 */
//#if (SYSTEM_SUPPORT_OS == 0U)
#if 1
        delayms(30);
#else
        delayxms(30);
#endif
        if (GUI_Read_KeyboardValue() != key) {
                key = 0xFF;
        }
        /* 等待 */
#if 1
        delayms(45);
#else
        delayxms(45);
#endif
        return key;
}
