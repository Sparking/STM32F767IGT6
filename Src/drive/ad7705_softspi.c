#include "ad7705.h"
#include "lcd.h"
#include "delay.h"

void AD7705_SoftSPI_SoftWaitDRDY(SoftSPI_DeviceTypeDef *dev)
{
        unsigned char reg;
        
        do {
                SoftSPI_HalfDuplex_WriteByte(dev, AD7705_REG_CMN | AD7705_CMD_READ);
                SoftSPI_HalfDuplex_ReadByte(dev, &reg);
                delayxms(1);
        } while (reg & AD7705_DRAY_STATUS);
        
}

void AD7705_SoftSPI_Register(SoftSPI_DeviceTypeDef *dev)
{
        dev->node->SCLK.GPIO = AD7705_SCLK_GPIO;
        dev->node->SCLK.Pin = AD7705_SCLK_PIN;
        dev->node->MISO.GPIO = AD7705_DOUT_GPIO;
        dev->node->MISO.Pin = AD7705_DOUT_PIN;
        dev->node->MOSI.GPIO = AD7705_DIN_GPIO;
        dev->node->MOSI.Pin = AD7705_DIN_PIN;
}

void AD7705_SoftSPI_ConfigCLK(SoftSPI_DeviceTypeDef *dev)
{
        /* 下个操作向时钟寄存器写入配置 */
        SoftSPI_HalfDuplex_WriteByte(dev, AD7705_REG_CLK | AD7705_CMD_WRITE);
        /* 配置为4.9152MHz外部晶振,2分频, 输出更新速率为100Hz */
        SoftSPI_HalfDuplex_WriteByte(dev, AD7005_CLKDIV_2 | AD7705_CLKSRC_1
                | AD7705_FILTER_SELECT(3));
}

void AD7705_SoftSPI_Calibration(SoftSPI_DeviceTypeDef *dev)
{
        SoftSPI_HalfDuplex_WriteByte(dev, AD7705_REG_CFG | AD7705_CMD_WRITE); /* 下个操作写入配置寄存器 */
        /* 自校准模式,增益为16,单极性输入,开缓冲 */
        SoftSPI_HalfDuplex_WriteByte(dev,  AD7705_SELF_CALIBRATION | AD7705_GAIN_16
                | AD7705_UNIPOLAR);
}

void AD7705_SoftSPI_SoftReset(SoftSPI_DeviceTypeDef *dev)
{
        unsigned char i;
        
        SoftSPI_NSS_Reset(dev);
        SoftSPI_MOSI_Set(dev);
        for (i = 0; i < 40; i++) {
                SoftSPI_SCLK_Reset(dev);
                SoftSPI_SCLK_Set(dev);
        }
        SoftSPI_NSS_Set(dev);
        /* 复位后都要重新设置一下寄存器 */
        AD7705_SoftSPI_ConfigCLK(dev);
        AD7705_SoftSPI_Calibration(dev);
        delayxms(1);
}

void AD7705_SoftSPI_Init(SoftSPI_DeviceTypeDef *dev)
{
        SoftSPI_SCLK_Set(dev);
        SoftSPI_MOSI_Set(dev);
        AD7705_SoftSPI_SoftReset(dev);
}

unsigned short AD7705_SoftSPI_ReadData(SoftSPI_DeviceTypeDef *dev)
{
        unsigned int data;

        SoftSPI_HalfDuplex_WriteByte(dev, AD7705_REG_CMN | AD7705_CMD_READ);
        SoftSPI_HalfDuplex_ReadData(dev, &data, 1);
        if (data == 0x88U) {
                AD7705_SoftSPI_SoftWaitDRDY(dev);
        }
        else if (data != 0x08U) {
                AD7705_SoftSPI_SoftReset(dev);
                AD7705_SoftSPI_SoftWaitDRDY(dev);
        }
        SoftSPI_HalfDuplex_WriteByte(dev, AD7705_REG_DAT | AD7705_CMD_READ);
        SoftSPI_HalfDuplex_ReadData(dev, &data, 2);
        return data;
}
