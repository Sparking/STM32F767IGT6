#ifndef __STM32F767_USER_LCD_H_
#define __STM32F767_USER_LCD_H_

#include "lcd.h"
#include "lcd_touch.h"
#include "user_lcd_touch.h"

extern LCD_WindowTypeDef GUI_Keyboard_KeyWin[12];
extern char *const GUI_Keyboard_KeyValue[12];

void GUI_Draw_Window(unsigned int start_pos_x, unsigned int start_pos_y,
        unsigned int end_pos_x, unsigned int end_pos_y,
        LCD_DirectionTypeDef lcd_dir, LCD_PixelTypeDef win_color,
        LCD_LayerTypeDef lcd_layer);
void GUI_Draw_ButtonWithText(char *string, const Font_TypeDef *font,
        unsigned int start_pos_x, unsigned int start_pos_y,
        unsigned int end_pos_x, unsigned int end_pos_y,
        unsigned int string_offset_x, unsigned int string_offset_y,
        LCD_DirectionTypeDef lcd_dir, LCD_PixelTypeDef win_color,
        LCD_PixelTypeDef font_color, LCD_LayerTypeDef lcd_layer);
void GUI_Draw_DigitalKeyboard(unsigned int key_width, unsigned int key_heigh,
        unsigned int key_x_offset, unsigned int key_y_offset,
        LCD_PixelTypeDef Key_bgcolor, LCD_PixelTypeDef key_color);
unsigned char GUI_Is_TouchInWindow(LCD_WindowTypeDef *win);
unsigned char GUI_Read_KeyboardValue(void);
unsigned char GUI_Read_KeyboardValueSteady(void);

#endif
