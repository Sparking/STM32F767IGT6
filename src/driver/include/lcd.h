#ifndef __LCD_CONFIG_H_
#define __LCD_CONFIG_H_

#include "stm32f7xx.h"
#include "stm32f7xx_hal_rcc.h"
#include "stm32f7xx_hal_cortex.h"
#include "stm32f7xx_hal_dma2d.h"
#include "stm32f7xx_hal_gpio.h"
#include "stm32f7xx_hal_ltdc.h"

#include "sdram.h"
#include "font.h"

#define LCD_BG_COLOR  0xFCC3U

struct __ltdc_devicetypedef{
        uint8_t  hbp;      /* 水平后沿宽度 */
        uint8_t  vbp;      /* 垂直后沿宽度 */
        uint8_t  hfp;      /* 水平前沿宽度 */
        uint8_t  vfp;      /* 垂直前沿宽度 */
        uint16_t hsync;    /* 水平同步宽度 */
        uint16_t vsync;    /* 垂直同步宽度 */
        uint16_t active_width; /* 有效宽度 */
        uint16_t active_heigh; /* 有效高度 */
};

extern const struct __ltdc_devicetypedef __ltdc_lcd_device;

typedef enum {
        LCD_LAYER_1 = 0U,  /* 第一层显示层 */
        LCD_LAYER_2 = 1U   /* 第二层显示层 */
} LCD_LayerTypeDef;

typedef enum {
        LCD_DIRECTION_H = 0U, /* 横屏显示 */
        LCD_DIRECTION_V = 1U  /* 竖屏显示 */
} LCD_DirectionTypeDef;

/*
 * 不论是横屏显示还是竖屏显示，X轴的方向都是横向朝右
 * Y轴的方向都是竖直朝下。
 */
typedef struct {
        unsigned int X;
        unsigned int Y;
        /* X轴(水平轴,方向朝右), Y轴(竖直轴,方向朝下) */
} LCD_PointPositionTypeDef;

typedef struct {
        LCD_LayerTypeDef Layer;  /* 显示层设置 */
        /* 横竖屏显示设置 */
        LCD_DirectionTypeDef Direction;
        LCD_PointPositionTypeDef StartPosition, EndPosition;
} LCD_WindowTypeDef;

typedef struct {
        unsigned int Color; /* 颜色 */
        LCD_LayerTypeDef Layer;  /* 显示层设置 */
        LCD_DirectionTypeDef Direction; /* 横竖屏设置 */
        LCD_PointPositionTypeDef Position; /* 点的位置 */
} LCD_PointTypeDef; /* 显示位置初始化结构体 */

typedef struct {
        LCD_PointTypeDef PointConfig; /* 指定了位置及显示的颜色 */
        struct {
                const Font_TypeDef *Font; /* 使用的字体 */
                unsigned int BackColor; /* 字体的背景色 */
                enum {
                        FONT_DISPLAY_WITH_BACK_COLOR    = 0U, /* 打印字体需要有背景色 */
                        FONT_DISPLAY_WITHOUT_BACK_COLOR = 1U  /* 字体显示无背景色 */
                } Mode; /* 字体显示模式 */
        } FontConfig; /* 字体配置 */
} LCD_CharConfigTypeDef;

/*
 * 像素格式
 *   000:ARGB8888
 *   001:RGB888
 *   010:RGB565
 *   011:ARGB1555
 *   100:ARGB4444
 *   101:L8(8 位 Luminance)
 *   110:AL44(4 位 Alpha,4 位 Luminance)
 *   111:AL88(8 位 Alpha,8 位 Luminance)
 */
#define LTDC_PIXEL_FORMAT 0x00000002 /* LCD像素格式 */
#define LTDC_LAYER_2_ENABLE 0U  /* 禁止LTDC第二层 */

/* 显示缓冲配置 */
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
#define LTDC_LAYERS_NUM 2U      /* 设置显存位置为两片缓冲区 */
#else
#define LTDC_LAYERS_NUM 1U      /* 设置显存位置为一片缓冲区 */
#endif

/* LCD显存设置 */
#define LTDC_PIXEL_SIZE 16         /* LCD像素16 bits */
typedef uint16_t LCD_PixelTypeDef; /* LCD像素类型定义 */
#define LCD_FRAME_BUFF_ADDRESS 0xC0000000U /* LCD显存地址 */

/*
 * 函数LCD_Init 初始化LCD
 */
void LCD_Init(void);

/*
 * 函数LCD_Draw_Point LCD中的画点函数
 */
void LCD_Draw_Point(LCD_PointTypeDef *hlp);

/*
 * 函数LCD_Puts LCD打印字符串
 * 参数hld:
 *      类型为LCD_CharConfigTypeDef *
 *  String(表示字符串),函数LCD_Puts只打印成员String而不打印Charactor
 */
unsigned int LCD_Puts(LCD_CharConfigTypeDef *hld, char *string);

/*
 * 函数LCD_Putc LCD打印字符
 */
unsigned int LCD_Putc(LCD_CharConfigTypeDef *hld, char ch);

/*
 * 函数LCD_Fill_Windows LCD窗口填充函数
 * 以一种颜色来填充指定的窗口
 */
void LCD_Fill_Window(const LCD_WindowTypeDef *hlfw, unsigned int color);

/*
 * 函数LCD_Clear LCD清屏函数
 */
void LCD_Clear(unsigned int color);

/*
 * 函数LCD_BlockCopy LCD显示区域拷贝函数
 * 作用:
 *  将某一区域的内容复制到另一个区域中
 */
void LCD_BlockCopy(const LCD_WindowTypeDef *src,
                   const LCD_WindowTypeDef *dist);

/*
 * 函数LCD_Flush
 * 作用:
 *  刷新DMA2D的输出，防止输出结果被DMA2D填充
 */
 void LCD_Flush(void);
 
#endif
