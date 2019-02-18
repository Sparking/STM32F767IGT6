#include "oled.h"

/* OLED_IIC_WriteByte
 * @dev: IIC Device
 * @reg: OLED Controller Register
 * @dat: data to write to the Register reg
 */
void OLED_IIC_WriteByte(i2c_device_t *dev, unsigned char reg,
        unsigned char dat)
{
        IIC_Start(dev);
        IIC_WriteByte(dev, dev->addr);
        IIC_WaitAck(dev);
        IIC_WriteByte(dev, reg);
        IIC_WaitAck(dev);
        IIC_WriteByte(dev, dat);
        IIC_WaitAck(dev);
        IIC_Stop(dev);
}

void OLED_IIC_On(i2c_device_t *dev)
{
        OLED_IIC_WRITECMD(dev, 0x8d);
        OLED_IIC_WRITECMD(dev, 0x14);
        OLED_IIC_WRITECMD(dev, 0xaf);
}

void OLED_IIC_Init(i2c_device_t *dev)
{
        delayxms(100);
        OLED_IIC_WRITECMD(dev, 0xAE);
        OLED_IIC_WRITECMD(dev, 0x20);
        OLED_IIC_WRITECMD(dev, 0x10);
        OLED_IIC_WRITECMD(dev, 0xb0);
        OLED_IIC_WRITECMD(dev, 0xc8);
        OLED_IIC_WRITECMD(dev, 0x00);
        OLED_IIC_WRITECMD(dev, 0x10);
        OLED_IIC_WRITECMD(dev, 0x40);
        OLED_IIC_WRITECMD(dev, 0x81);
        OLED_IIC_WRITECMD(dev, 0xff);
        OLED_IIC_WRITECMD(dev, 0xa1);
        OLED_IIC_WRITECMD(dev, 0xa6);
        OLED_IIC_WRITECMD(dev, 0xa8);
        OLED_IIC_WRITECMD(dev, 0x3F);
        OLED_IIC_WRITECMD(dev, 0xa4);
        OLED_IIC_WRITECMD(dev, 0xd3);
        OLED_IIC_WRITECMD(dev, 0x00);
        OLED_IIC_WRITECMD(dev, 0xd5);
        OLED_IIC_WRITECMD(dev, 0xf0);
        OLED_IIC_WRITECMD(dev, 0xd9);
        OLED_IIC_WRITECMD(dev, 0x22);
        OLED_IIC_WRITECMD(dev, 0xda);
        OLED_IIC_WRITECMD(dev, 0x12);
        OLED_IIC_WRITECMD(dev, 0xdb);
        OLED_IIC_WRITECMD(dev, 0x20);
        OLED_IIC_On(dev);
}

void OLED_IIC_SetPos(i2c_device_t *dev, unsigned char x,
        unsigned char y)
{
        OLED_IIC_WRITECMD(dev, 0xb0 + y);
        OLED_IIC_WRITECMD(dev, (x >> 4) | 0x10);
        OLED_IIC_WRITECMD(dev, (x & 0x0f) | 0x01);
}

void OLED_IIC_Clean(i2c_device_t *dev, unsigned char data)
{
        unsigned char m, n;
        for (m = 0; m < 8; m++) {
                OLED_IIC_WRITECMD(dev, 0xb0 + m);
                OLED_IIC_WRITECMD(dev, 0x00);
                OLED_IIC_WRITECMD(dev, 0x10);
                for (n = 0; n < 128; n++)
                        OLED_IIC_WRITEDATA(dev, data);
        }
}
/*
void OLED_IIC_Puts(i2c_device_t *dev, char *string, Font_TypeDef *font,
        unsigned char x, unsigned char y)
{
        unsigned char ch;
        unsigned char i;
        
        for (; *string != 0; string++) {
                ch = *string - 32;
                OLED_IIC_SetPos(dev, x, y);
                for (i = 0; i < font->Width; i++)
                        OLED_IIC_WRITEDATA(font->Table[ch][i]);
                x += font->Width;
        }
}
*/