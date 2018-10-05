#ifndef __OLED_H_
#define __OLED_H_

#include "iic.h"
#include "font.h"

#define OLED_IIC_ADDR  0x78

/* 发送命令 */
#define OLED_IIC_WRITECMD(dev,cmd)      \
        OLED_IIC_WriteByte(dev, 0x00U, cmd)
/* 写入数据 */
#define OLED_IIC_WRITEDATA(dev, data)   \
        OLED_IIC_WriteByte(dev, 0x40U, data)

void OLED_IIC_WriteByte(i2c_device_t *dev, unsigned char reg,
        unsigned char dat);
void OLED_IIC_WriteCmd(i2c_device_t *dev, unsigned char cmd);
void OLED_IIC_Init(i2c_device_t *dev);
void OLED_IIC_SetPos(i2c_device_t *dev, unsigned char x,
        unsigned char y);
void OLED_IIC_Clean(i2c_device_t *dev, unsigned char data);

#endif /* End of __OLED_H_ */
