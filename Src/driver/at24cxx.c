#include "at24cxx.h"

void AT24CXX_WriteByte(i2c_device_t *at24cxx_dev, unsigned short addr, unsigned char data)
{
    IIC_Start(at24cxx_dev);
    if (EE_TYPE > AT24C16) {
        IIC_WriteByte(at24cxx_dev, at24cxx_dev->addr);
        IIC_WaitAck(at24cxx_dev);
        IIC_WriteByte(at24cxx_dev, addr >> 8);
    }
    else
        IIC_WriteByte(at24cxx_dev, at24cxx_dev->addr + ((addr / 256) << 1));
    IIC_WaitAck(at24cxx_dev);
    IIC_WriteByte(at24cxx_dev, addr % 256);
    IIC_WaitAck(at24cxx_dev);
    IIC_WriteByte(at24cxx_dev, data);
    IIC_WaitAck(at24cxx_dev);
    IIC_Stop(at24cxx_dev);
    delayxms(10);
}

void AT24CXX_WriteLenByte(i2c_device_t *at24cxx_dev, unsigned short addr, unsigned int data, unsigned char data_len)
{
    for (unsigned char i = 0; i < data_len; i++)
        AT24CXX_WriteByte(at24cxx_dev, addr, (data >> (8 * i)) & 0xFFU);
}

void AT24CXX_WriteBuff(i2c_device_t *at24cxx_dev, unsigned short addr, unsigned char *buff, unsigned short len)
{
    for (unsigned short i = 0; i < len; i++)
        AT24CXX_WriteByte(at24cxx_dev, addr + i, buff[i]);
}

void AT24CXX_ReadByte(i2c_device_t *at24cxx_dev, unsigned short addr, unsigned char *pdata)
{
    IIC_Start(at24cxx_dev);
    if (EE_TYPE > AT24C16) {
        IIC_WriteByte(at24cxx_dev, at24cxx_dev->addr);
        IIC_WaitAck(at24cxx_dev);
        IIC_WriteByte(at24cxx_dev, addr >> 8); /* 发送高地址 */
    }
    else
        IIC_WriteByte(at24cxx_dev, at24cxx_dev->addr + ((addr / 256) << 1));
    IIC_WaitAck(at24cxx_dev);
    IIC_WriteByte(at24cxx_dev, addr % 256);
    IIC_WaitAck(at24cxx_dev);
    IIC_Start(at24cxx_dev);
    IIC_WriteByte(at24cxx_dev, 0xA1);
    IIC_WaitAck(at24cxx_dev);
    *pdata = IIC_ReadByte(at24cxx_dev, 0);
    IIC_Stop(at24cxx_dev);
}

void AT24CXX_ReadLenByte(i2c_device_t *at24cxx_dev, unsigned short addr, unsigned int *pdata, unsigned char data_len)
{
    unsigned int data = 0;
    unsigned char temp;

    for (unsigned char i = 0; i < data_len; i++) {
        data <<= 8;
        AT24CXX_ReadByte(at24cxx_dev, addr + data_len - i - 1, &temp);
        data |= temp;
    }
    *pdata = data;
}

void AT24CXX_ReadBuff(i2c_device_t *at24cxx_dev, unsigned short addr, unsigned char *buff, unsigned short len)
{
    for (unsigned short i = 0; i < len; i++)
        AT24CXX_ReadByte(at24cxx_dev, addr + i, buff + i);
}

unsigned char AT24CXX_Check(i2c_device_t *at24cxx_dev)
{
    unsigned char data;
    unsigned char ret = 0U;
    AT24CXX_ReadByte(at24cxx_dev, 255U, &data);
    if (data == 0x55U)
        ret = 1U;
    else {
        AT24CXX_WriteByte(at24cxx_dev, 255U, 0x55U);
        AT24CXX_ReadByte(at24cxx_dev, 255U, &data);
        if (data == 0x55U)
            ret = 1U;
    }
    return ret;
}

