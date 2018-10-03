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

void OLED_IIC_WriteByte(IIC_DeviceTypeDef *dev, unsigned char reg,
        unsigned char dat);
void OLED_IIC_WriteCmd(IIC_DeviceTypeDef *dev, unsigned char cmd);
void OLED_IIC_Init(IIC_DeviceTypeDef *dev);
void OLED_IIC_SetPos(IIC_DeviceTypeDef *dev, unsigned char x,
        unsigned char y);
void OLED_IIC_Clean(IIC_DeviceTypeDef *dev, unsigned char data);

#endif /* End of __OLED_H_ */
