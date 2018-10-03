#include "at24cxx.h"

void AT24CXX_WriteByte(IIC_DeviceTypeDef *_at24cxx_dev, unsigned short addr, unsigned char data)
{
        IIC_Start(_at24cxx_dev);
        if (EE_TYPE > AT24C16) {
                IIC_WriteByte(_at24cxx_dev, _at24cxx_dev->IIC_Device_Address);
                IIC_WaitAck(_at24cxx_dev);
                IIC_WriteByte(_at24cxx_dev, addr >> 8);
        }
        else
                IIC_WriteByte(_at24cxx_dev, _at24cxx_dev->IIC_Device_Address + ((addr / 256) << 1));
        IIC_WaitAck(_at24cxx_dev);
        IIC_WriteByte(_at24cxx_dev, addr % 256);
        IIC_WaitAck(_at24cxx_dev);
        IIC_WriteByte(_at24cxx_dev, data);
        IIC_WaitAck(_at24cxx_dev);
        IIC_Stop(_at24cxx_dev);
        delayxms(10);
}

void AT24CXX_WriteLenByte(IIC_DeviceTypeDef *_at24cxx_dev, unsigned short addr, unsigned int data, unsigned char data_len)
{
        for (unsigned char i = 0; i < data_len; i++)
                AT24CXX_WriteByte(_at24cxx_dev, addr, (data >> (8 * i)) & 0xFFU);
}

void AT24CXX_WriteBuff(IIC_DeviceTypeDef *_at24cxx_dev, unsigned short addr, unsigned char *buff, unsigned short len)
{
        for (unsigned short i = 0; i < len; i++)
                AT24CXX_WriteByte(_at24cxx_dev, addr + i, buff[i]);
}

void AT24CXX_ReadByte(IIC_DeviceTypeDef *_at24cxx_dev, unsigned short addr, unsigned char *pdata)
{
        IIC_Start(_at24cxx_dev);
        if (EE_TYPE > AT24C16) {
                IIC_WriteByte(_at24cxx_dev, _at24cxx_dev->IIC_Device_Address);
                IIC_WaitAck(_at24cxx_dev);
                IIC_WriteByte(_at24cxx_dev, addr >> 8); /* 发送高地址 */
        }
        else
                IIC_WriteByte(_at24cxx_dev, _at24cxx_dev->IIC_Device_Address + ((addr / 256) << 1));
        IIC_WaitAck(_at24cxx_dev);
        IIC_WriteByte(_at24cxx_dev, addr % 256);
        IIC_WaitAck(_at24cxx_dev);
        IIC_Start(_at24cxx_dev);
        IIC_WriteByte(_at24cxx_dev, 0xA1);
        IIC_WaitAck(_at24cxx_dev);
        *pdata = IIC_ReadByte(_at24cxx_dev, 0);
        IIC_Stop(_at24cxx_dev);
}

void AT24CXX_ReadLenByte(IIC_DeviceTypeDef *_at24cxx_dev, unsigned short addr, unsigned int *pdata, unsigned char data_len)
{
        unsigned int data = 0;
        unsigned char temp;
        for (unsigned char i = 0; i < data_len; i++) {
                data <<= 8;
                 AT24CXX_ReadByte(_at24cxx_dev, addr + data_len - i - 1, &temp);
                data |= temp;
        }
        *pdata = data;
}

void AT24CXX_ReadBuff(IIC_DeviceTypeDef *_at24cxx_dev, unsigned short addr, unsigned char *buff, unsigned short len)
{
        for (unsigned short i = 0; i < len; i++)
                AT24CXX_ReadByte(_at24cxx_dev, addr + i, buff + i);
}

unsigned char AT24CXX_Check(IIC_DeviceTypeDef *_at24cxx_dev)
{
        unsigned char data;
        unsigned char ret = 0U;
        AT24CXX_ReadByte(_at24cxx_dev, 255U, &data);
        if (data == 0x55U)
                ret = 1U;
        else {
                AT24CXX_WriteByte(_at24cxx_dev, 255U, 0x55U);
                AT24CXX_ReadByte(_at24cxx_dev, 255U, &data);
                if (data == 0x55U)
                        ret = 1U;
        }
        return ret;
}
