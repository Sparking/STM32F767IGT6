#include "lcd.h"

const struct __ltdc_devicetypedef __ltdc_lcd_device = { /* 方便统一修改 */
        .hsync        = 1,
        .vsync        = 1,
        .hbp          = 40,
        .vbp          = 8,
        .hfp          = 5,
        .vfp          = 8,
        .active_width = 480,
        .active_heigh = 272
};

#define __lcd_frame_buff ((LCD_PixelTypeDef *)(LCD_FRAME_BUFF_ADDRESS));

void LCD_Init(void)
{
        uint16_t ltdc_hconfig_value, ltdc_vconfig_value;
        HAL_LTDC_MspInit(NULL);
/**** 同步信号和时钟极性配置 ****/
/* LTDC->GCR 全局控制寄存器
 * HSPOL[31]:水平同步极性 0:低电平有效 1:高电平有效
 * VSPOL[30]:垂直同步极性 0:低电平有效 1:高电平有效
 * DEPOL[29]:数据使能极性 0:低电平有效 1:高电平有效
 * PCPOL[28]:像素时钟极性 0:低电平有效 1:高电平有效
 * DEN[16]:    抖动使能 0:禁止 1:使能
 * DRW[14:12]: 抖动红色宽度
 * DGW[10:8]:  抖动绿色宽度
 * DBW[6:4]:   抖动蓝色宽度
 * LTDCEN[0]: LCD-TFT控制器使能
 */
        LTDC->GCR = 0x00000001; /* 最后再使能,先配置同步信号和时钟极性 */
/**** LCD 同步时序配置 ****/
        /* LTDC同步周期大小配置 */
        ltdc_hconfig_value = __ltdc_lcd_device.hsync - 1;
        ltdc_vconfig_value = __ltdc_lcd_device.vsync - 1;
        LTDC->SSCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
        /* LTDC后沿配置 */
        ltdc_hconfig_value += __ltdc_lcd_device.hbp;
        ltdc_vconfig_value += __ltdc_lcd_device.vbp;
        LTDC->BPCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
        /* LTDC有效宽度配置 */
        ltdc_hconfig_value += __ltdc_lcd_device.active_width;
        ltdc_vconfig_value += __ltdc_lcd_device.active_heigh;
        LTDC->AWCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
        /* LTDC总宽度配置 */
        ltdc_hconfig_value += __ltdc_lcd_device.hfp;
        ltdc_vconfig_value += __ltdc_lcd_device.vfp;
        LTDC->TWCR = ((ltdc_hconfig_value) << 16) | (ltdc_vconfig_value);
/**** 背景色配置 ****/
/* LTDC->BCCR 背景色控制寄存器
 * BCRED[23:16]:  背景红色值
 * BCGREEN[15:8]: 背景绿色值
 * BCBLUE[7:0]:   背景蓝色值
 */
        LTDC->BCCR = 0;
/**** 中断配置 ****/
        /* LTDC中断配置(全禁用) */
        LTDC->IER = 0;
        /* LTDC行中断位置配置 */
        LTDC->LIPCR = 0;

/************ LTDC层参数配置 ***************/
/**** LTDC窗口水平位置和垂直位置配置 ****/
/* LTDC_Layerx->WHPCR 层窗口水平位置配置
 * WHSPPOS[27:16]: 窗口水平结束位置
 * WHSTPOS[11:0]:  窗口水平起始位置
 */
        ltdc_hconfig_value = __ltdc_lcd_device.hsync + __ltdc_lcd_device.hbp;
        LTDC_Layer1->WHPCR = 
                ((ltdc_hconfig_value + __ltdc_lcd_device.active_width - 1)
                << 16) | ltdc_hconfig_value;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
        LTDC_Layer2->WHPCR =
                ((ltdc_hconfig_value + __ltdc_lcd_device.active_width - 1)
                << 16) | ltdc_hconfig_value;
#endif
/* LTDC_Layerx->WVPCR 层窗口垂直位置配置
 * WCSPPOS[26:16]: 窗口垂直结束位置
 * WVSTPOS[10:0]:  窗口垂直起始位置
 */
        ltdc_vconfig_value = __ltdc_lcd_device.vsync + __ltdc_lcd_device.vbp;
        LTDC_Layer1->WVPCR =
                ((ltdc_vconfig_value + __ltdc_lcd_device.active_heigh - 1)
                << 16) | ltdc_vconfig_value;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
        LTDC_Layer2->WVPCR =
                ((ltdc_vconfig_value + __ltdc_lcd_device.active_heigh - 1)
                << 16) | ltdc_vconfig_value;
#endif
/**** LTDC层像素输入格式 ****/
/* LTDC_Layerx->PFCR
 * PF[2:0]:像素格式
 */
        LTDC_Layer1->PFCR  = LTDC_PIXEL_FORMAT;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
        LTDC_Layer2->PFCR  = LTDC_PIXEL_FORMAT;
#endif
/**** LTDC颜色帧配置 ****/
/* LTDC_Layerx->CFBAR 颜色帧缓冲区地址
 * CFBADD[31:0]:颜色帧缓冲区起始地址 (Color Frame Buffer Start Address)
 *   这些位定义颜色帧缓冲区的起始地址。
 *
 * LTDC_Layerx->CFBLR 颜色帧缓冲区长度
 * CFBP[28:16]: 颜色帧缓冲区间距(以字节为单位)
 *   这些位定义从像素某行的起始处到下一行的起始处的增量(以字节为单位)。
 * CFBLL[12:0]: 颜色帧缓冲区行长
 *   这些位定义一行像素的长度(以字节为单位)+ 3。
 *   行长的计算方法为:有效宽度 x 每像素的字节数 + 3。
 *
 * LTDC_Layerx->CFBLNR 颜色帧缓冲区行数
 * CFBLNBR[10:0]:帧缓冲区行数
 *   这些位用来定义缓冲区中的行数,行数跟有效高度对应。
 */
        LTDC_Layer1->CFBAR  = LCD_FRAME_BUFF_ADDRESS;       /* LCD显存位置 */
        /* 480 * 2(RGB565) => 0x03C0 */
        ltdc_hconfig_value  = __ltdc_lcd_device.active_width * 2;
        LTDC_Layer1->CFBLR  = (ltdc_hconfig_value << 16)
                | (ltdc_hconfig_value + 3); /* 0x03C003C3 */
        LTDC_Layer1->CFBLNR = __ltdc_lcd_device.active_heigh;     /* 272 */
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
        LTDC_Layer2->CFBAR  = LCD_FRAME_BUFF_ADDRESS
                + __ltdc_lcd_device.active_heigh
                * __ltdc_lcd_device.active_width;
        ltdc_hconfig_value  = __ltdc_lcd_device.active_width * 2;
        LTDC_Layer2->CFBLR  = (ltdc_hconfig_value << 16)
                | (ltdc_hconfig_value + 3);
        LTDC_Layer2->CFBLNR = __ltdc_lcd_device.active_heigh;
#endif
/**** CLUT加载RGB值及其 ****/
/* LTDC_Layerx->CLUTWR CLUT写寄存器
 * CLUTADD[31:24]: CLUT地址
 *   这些位配置每个RGB值的CLUT地址(CLUT内的颜色位置)
 * RED[23:16]: 红色值
 *   这些位配置红色值
 * GREEN[15:8]: 绿色值
 *   这些位配置绿色值
 * BLUE[7:0]: 蓝色值
 *   这些位配置蓝色值
 * CLUT可通过LTDC_LxCR寄存器使能和禁止。
 * CLUT仅对L8、AL44和AL88像素格式有意义。
 */
/*       LTDC_Layer1->CLUTWR = 0; */
/*       LTDC_Layer2->CLUTWR = 0; */
/**** 默认颜色和混合系数配置 ****/
/* LTDC_Layerx->DCCR 默认颜色配置寄存器
 * DCALPHA[31:24]:默认Alpha值
 *   这些位配置默认alpha值
 * DCRED[23:16]:默认颜色红色
 *   这些位配置默认红色值
 * DCGREEN[15:8]:默认颜色绿色
 *   这些位配置默认绿色值
 * DCBLUE[7:0]:默认颜色蓝色
 *   这些位配置默认蓝色值
 *
 * LTDC_Layerx->BFCR 混合系数配置(配置详情请参考 STM32F7xx中文参考手册P498)
 *     LTDC_Layerx->CACR用于设置常数Alpha的值
 * 通用混合公式为:BC = BF1 x C + BF2 x Cs
 *  BC  = 混合后的颜色
 *  BF1 = 混合系数 1
 *  C   = 当前层颜色
 *  BF2 = 混合系数 2
 *  Cs  = 底层混合后的颜色
 * BF1[10:8]:取值如下
 *  100: 常数Alpha
 *  110: 像素Alpha*常数Alpha
 * BF2[2:0]:取值如下
 *  101: 1——常数Alpha
 *  111: 1——像素Alpha*常数Alpha
 */
        LTDC_Layer1->DCCR = 0x00000000;
        LTDC_Layer1->CACR = 0x000000FF; /* 常数Alpha = 配置值/255 */
        LTDC_Layer1->BFCR = 0x00000203;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
        LTDC_Layer2->DCCR = 0x00000000;
        LTDC_Layer2->CACR = 0x0000007F;
        LTDC_Layer2->BFCR = 0x00000203;
#endif
 /**** 使能层数 ****/
 /* LTDC_Layerx->CR 控制寄存器
  * CLUTEN[4]:查色表使能
  *   0:禁止查色表
  *   1:使能查色表
  *   CLUT 仅对 L8、AL44 和 AL88 像素格式有意义。请参见第 479 页上的查色表 (CLUT)。
  * COLKEN[1]:色键使能
  *   0:禁止色键
  *   1:使能色键
  * LEN[0]:层使能
  *   0:禁止层
  *   1:使能层
  */
        LTDC_Layer1->CR = 0x1;
#if (LTDC_LAYER_2_ENABLE != 0U) /* 如果有使能第二层 */
        LTDC_Layer2->CR = 0x1;
#endif
/**** 将影子寄存器重装载到活动寄存器 ****/
        LTDC->SRCR |= 0x1; /* 立即重载 */
/**** 使能LCD-TFT控制器 ****/
        LTDC->GCR  |= 0x1;
}

void HAL_LTDC_MspInit(LTDC_HandleTypeDef *hltdc)
{
        GPIO_InitTypeDef GPIO_Initure;
/****** GPIO 配置开始 ******/
/*
 * PB5: LCD_BL LCD背光
 */
        GPIO_Initure.Pin  = GPIO_PIN_5;
        GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed= GPIO_SPEED_LOW;
        GPIO_Initure.Alternate = 0;
        __HAL_RCC_GPIOB_CLK_ENABLE();
        HAL_GPIO_Init(GPIOB, &GPIO_Initure);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
/*
 * PF10: LCD_DE
 */
        GPIO_Initure.Pin  = GPIO_PIN_10;
        GPIO_Initure.Mode = GPIO_MODE_AF_PP;
        GPIO_Initure.Pull = GPIO_NOPULL;
        GPIO_Initure.Speed= GPIO_SPEED_HIGH;
        GPIO_Initure.Alternate = GPIO_AF14_LTDC;
        __HAL_RCC_GPIOF_CLK_ENABLE();
        HAL_GPIO_Init(GPIOF, &GPIO_Initure);
/*
 * PG6: LCD_R[7]
 * PG7: LCD_CLK
 * PG11: LCD_B[3]
 */
        GPIO_Initure.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_11;
        __HAL_RCC_GPIOG_CLK_ENABLE();
        HAL_GPIO_Init(GPIOG, &GPIO_Initure);
/*
 * PH9: LCD_R[3]
 * PH10: LCD_R[4]
 * PH11: LCD_R[5]
 * PH12: LCD_R[6]
 * PH13: LCD_G[2]
 * PH14: LCD_G[3]
 * PH15: LCD_G[4]
 */
        GPIO_Initure.Pin = GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11 | GPIO_PIN_12
                | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;
        __HAL_RCC_GPIOH_CLK_ENABLE();
        HAL_GPIO_Init(GPIOH, &GPIO_Initure);
/*
 * PI0: LCD_G[5]
 * PI1: LCD_G[6]
 * PI2: LCD_G[7]
 * PI4: LCD_B[4]
 * PI5: LCD_B[5]
 * PI6: LCD_B[6]
 * PI7: LCD_B[7]
 * PI9: LCD_VSYNC
 * PI10: LCD_HSYNC
 */
        GPIO_Initure.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4
                | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_9
                | GPIO_PIN_10;
        __HAL_RCC_GPIOI_CLK_ENABLE();
        HAL_GPIO_Init(GPIOI, &GPIO_Initure);
/****** GPIO 配置完成 ******/
        __HAL_RCC_LTDC_CLK_ENABLE();
        __HAL_RCC_DMA2D_CLK_ENABLE();
}

static LCD_PixelTypeDef *LCD_GetPointAddress(const LCD_LayerTypeDef layer,
        const LCD_PointPositionTypeDef pos, const LCD_DirectionTypeDef dir)
{
        LCD_PixelTypeDef *frame = __lcd_frame_buff;
        if (layer == LCD_LAYER_2)
                frame = (LCD_PixelTypeDef *)((uint32_t)frame +
                        __ltdc_lcd_device.active_heigh *
                        __ltdc_lcd_device.active_width);
        return (dir == LCD_DIRECTION_V) ?
              &frame[__ltdc_lcd_device.active_width *
                        (__ltdc_lcd_device.active_heigh - pos.X - 1) + pos.Y] :
              &frame[pos.Y * __ltdc_lcd_device.active_width + pos.X];
}

void LCD_Draw_Point(LCD_PointTypeDef *hldp)
{
        *LCD_GetPointAddress(hldp->Layer, hldp->Position, hldp->Direction)
                = hldp->Color;
}

unsigned int LCD_Putc(LCD_CharConfigTypeDef *hld, char ch)
{
        unsigned char cs, i; /* charactor(width) size */
        unsigned char ch_index = ch - ' ';
        unsigned int temp;
        unsigned int x_pos = hld->PointConfig.Position.X;
        unsigned int y_pos = hld->PointConfig.Position.Y;
        unsigned int bg_color = hld->FontConfig.BackColor;
        unsigned int char_color = hld->PointConfig.Color;

        if (hld->FontConfig.Font == NULL) {
                return x_pos;
        }
        for (cs = 0; cs < hld->FontConfig.Font->Width; cs ++) {
                temp = hld->FontConfig.Font->Table
                        [ch_index * hld->FontConfig.Font->Width + cs];
                for (i = 0; i < 8; i++) {
                        if (temp & 0x80) {
                                hld->PointConfig.Color = char_color;
                                LCD_Draw_Point(&hld->PointConfig);
                        }
                        else if(hld->FontConfig.Mode ==
                                FONT_DISPLAY_WITH_BACK_COLOR) {
                                hld->PointConfig.Color = bg_color;
                                LCD_Draw_Point(&hld->PointConfig);
                        }
                        temp <<= 1;
                        hld->PointConfig.Position.Y++;
                        if (hld->PointConfig.Position.Y - y_pos ==
                                hld->FontConfig.Font->Heigh) {
                                hld->PointConfig.Position.Y = y_pos;
                                hld->PointConfig.Position.X++;
                        }
                }
        }
        temp = hld->PointConfig.Position.X;
        hld->PointConfig.Position.X = x_pos;
        hld->PointConfig.Position.Y = y_pos;
        hld->PointConfig.Color = char_color;
        return temp;
}

unsigned int LCD_Puts(LCD_CharConfigTypeDef *hld, char *string)
{
        unsigned int temp;
        unsigned int index;
        unsigned int X_pos_backup = hld->PointConfig.Position.X;

        for (index = 0; string[index] != '\0'; index ++) {
                if ((temp = LCD_Putc(hld, string[index]))
                        == hld->PointConfig.Position.X)
                        break;
                hld->PointConfig.Position.X = temp;
        }
        hld->PointConfig.Position.X = X_pos_backup;
        return index;
}

static void _LCD_XY_Convert(const LCD_WindowTypeDef *hlfw,
        unsigned int *converted_x, unsigned int *converted_y,
        unsigned int *lines, unsigned int *line_size)
{
        if (hlfw->Direction == LCD_DIRECTION_V) {
                *lines     = hlfw->EndPosition.X - hlfw->StartPosition.X;
                *line_size = hlfw->EndPosition.Y - hlfw->StartPosition.Y;
                *converted_x = hlfw->StartPosition.Y;
                *converted_y = __ltdc_lcd_device.active_heigh
                        - hlfw->EndPosition.X;
        }
        else {
                *lines     = hlfw->EndPosition.Y - hlfw->StartPosition.Y;
                *line_size = hlfw->EndPosition.X - hlfw->StartPosition.X;
                *converted_x = hlfw->StartPosition.X;
                *converted_y = hlfw->StartPosition.Y;
        }
}

void LCD_Fill_Window(const LCD_WindowTypeDef *hlfw, unsigned int color)
{
        unsigned int lines, line_size;
        LCD_PointPositionTypeDef pos;

        LCD_Flush();
        DMA2D->CR = (3 << 16); /* 设置DMA2D的模式 */
        DMA2D->OPFCCR = LTDC_PIXEL_FORMAT; /* 输出像素格式为RGB565 */
        DMA2D->OCOLR = color; /* 填充的颜色 */
        _LCD_XY_Convert(hlfw, &pos.X, &pos.Y, &lines, &line_size);
        DMA2D->OMAR = (unsigned int)LCD_GetPointAddress(hlfw->Layer, pos,
                LCD_DIRECTION_H); /* 存储器地址 */
        DMA2D->OOR = __ltdc_lcd_device.active_width - line_size;
        DMA2D->NLR = lines + (line_size << 16);
        DMA2D->CR |= 0x0001;
}

void LCD_BlockCopy(const LCD_WindowTypeDef *src, const LCD_WindowTypeDef *dist)
{
        unsigned int lines, line_size;
        LCD_PointPositionTypeDef src_pos, dist_pos;

        LCD_Flush();
        _LCD_XY_Convert(src, &src_pos.X, &src_pos.Y, &lines, &line_size);
        DMA2D->FGPFCCR = LTDC_PIXEL_FORMAT;
        DMA2D->FGMAR = (unsigned int)LCD_GetPointAddress(src->Layer, src_pos,
                LCD_DIRECTION_H); /* 存储器地址 */
        DMA2D->FGOR = __ltdc_lcd_device.active_width - line_size;
        DMA2D->OOR = __ltdc_lcd_device.active_width - line_size;
        DMA2D->NLR = lines + (line_size << 16);
        _LCD_XY_Convert(dist, &dist_pos.X, &dist_pos.Y, &lines, &line_size);
        DMA2D->OMAR = (unsigned int)LCD_GetPointAddress(dist->Layer, dist_pos,
                LCD_DIRECTION_H); /* 存储器地址 */
        DMA2D->CR = 0x0001;
}

void LCD_Clear(unsigned int color)
{
        LCD_WindowTypeDef LCD_Fill_Handler;

/* LCD填充区域 */
        LCD_Fill_Handler.Layer = LCD_LAYER_1; /* 显示层设为第一层 */
        LCD_Fill_Handler.Direction = LCD_DIRECTION_H; /* 横屏显示 */
        LCD_Fill_Handler.StartPosition.X = 0; /* 开始位置X轴 */
        LCD_Fill_Handler.StartPosition.Y = 0; /* 开始位置Y轴 */
        LCD_Fill_Handler.EndPosition.X = 480; /* 结束位置X轴 */
        LCD_Fill_Handler.EndPosition.Y = 272; /* 结束位置Y轴 */
        LCD_Fill_Window(&LCD_Fill_Handler, color);  /* 开始填充 */
        LCD_Flush();
}

void LCD_Flush(void)
{
        unsigned int time = 0;

        while (DMA2D->CR & 0x0001 && time++ < (~0U))
                continue;
}
