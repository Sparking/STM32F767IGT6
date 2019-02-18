#include "softspi.h"
#include "delay.h"

void SoftSPI_Init(SoftSPI_DeviceTypeDef *dev)
{
        GPIO_InitTypeDef GPIO_Init;

        GPIO_Clk_Enable(dev->node->SCLK.GPIO);
        GPIO_Clk_Enable(dev->node->MISO.GPIO);
        GPIO_Clk_Enable(dev->node->MOSI.GPIO);
        GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init.Pull = GPIO_PULLUP;
        GPIO_Init.Speed= GPIO_SPEED_FREQ_HIGH;
        GPIO_Init.Pin = dev->node->SCLK.Pin;
        HAL_GPIO_Init(dev->node->SCLK.GPIO, &GPIO_Init);
        GPIO_Init.Pin = dev->node->MOSI.Pin;
        HAL_GPIO_Init(dev->node->MOSI.GPIO, &GPIO_Init);
        GPIO_Init.Mode = GPIO_MODE_INPUT;
        GPIO_Init.Pin = dev->node->MISO.Pin;
        HAL_GPIO_Init(dev->node->MISO.GPIO, &GPIO_Init);
}

void SoftSPI_HalfDuplex_WriteByte(SoftSPI_DeviceTypeDef *dev,
        unsigned char data)
{
        unsigned char i;

        for (i = 0; i < 8; i++) {
                if (data & 0x80)
                        SoftSPI_MOSI_Set(dev);
                else
                        SoftSPI_MOSI_Reset(dev);
                SoftSPI_SCLK_Reset(dev);
                SoftSPI_SCLK_Set(dev);
                data <<= 1;
        }
        SoftSPI_MOSI_Set(dev);
}

void SoftSPI_HalfDuplex_WriteData(SoftSPI_DeviceTypeDef *dev,
        unsigned int data, unsigned char size)
{
        for (size = 1 << (size * 8 - 1); size != 0; size >>= 1) {
                if (data & size)
                        SoftSPI_MOSI_Set(dev);
                else
                        SoftSPI_MOSI_Reset(dev);
                SoftSPI_SCLK_Reset(dev);
                SoftSPI_SCLK_Set(dev);
                data <<= 1;
        }
        SoftSPI_MOSI_Set(dev);
}

void SoftSPI_HalfDuplex_ReadByte(SoftSPI_DeviceTypeDef *dev,
        unsigned char *data)
{
        unsigned char i;

        for (i = 0; i < 8; i++) {
                *data <<= 1;
                SoftSPI_SCLK_Reset(dev);
                SoftSPI_SCLK_Set(dev);
                *data += SoftSPI_MISO_Value(dev);
        }
        SoftSPI_MOSI_Set(dev);
}

void SoftSPI_HalfDuplex_ReadData(SoftSPI_DeviceTypeDef *dev,
        unsigned int *data, unsigned char size)
{
        unsigned char i;

        for (*data = 0, i = 0, size *= 8; i < size; i++) {
                *data <<= 1;
                SoftSPI_SCLK_Reset(dev);
                SoftSPI_SCLK_Set(dev);
                *data += SoftSPI_MISO_Value(dev);
        }
        SoftSPI_MOSI_Set(dev);
}
