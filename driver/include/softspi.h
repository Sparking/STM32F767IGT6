#ifndef __SOFTSPI_H_
#define __SOFTSPI_H_

#include "gpio.h"

typedef struct __soft_spi_node_t {
        Single_GPIOPinTypeDef SCLK;
        Single_GPIOPinTypeDef MISO;
        Single_GPIOPinTypeDef MOSI;
} SoftSPI_NodeTypeDef;

typedef struct __soft_spi_device_t {
        SoftSPI_NodeTypeDef *node;
        //Single_GPIOPinTypeDef NSS;
} SoftSPI_DeviceTypeDef;

#define SoftSPI_Pin_Set(dev,pin) \
        HAL_GPIO_WritePin((dev)->node->pin.GPIO, (dev)->node->pin.Pin, GPIO_PIN_SET)
#define SoftSPI_Pin_Reset(dev,pin) \
        HAL_GPIO_WritePin((dev)->node->pin.GPIO, (dev)->node->pin.Pin, GPIO_PIN_RESET)
#define SoftSPI_MISO_Value(dev) \
        HAL_GPIO_ReadPin((dev)->node->MISO.GPIO, (dev)->node->MISO.Pin)
#define SoftSPI_NSS_Set(dev)
//        HAL_GPIO_WritePin((dev)->NSS.GPIO, (dev)->NSS.Pin, GPIO_PIN_SET)
#define SoftSPI_NSS_Reset(dev)
//        HAL_GPIO_WritePin((dev)->NSS.GPIO, (dev)->NSS.Pin, GPIO_PIN_RESET)
#define SoftSPI_SCLK_Set(dev)   SoftSPI_Pin_Set(dev,SCLK)
#define SoftSPI_SCLK_Reset(dev) SoftSPI_Pin_Reset(dev,SCLK)
#define SoftSPI_MOSI_Set(dev)   SoftSPI_Pin_Set(dev,MOSI)
#define SoftSPI_MOSI_Reset(dev) SoftSPI_Pin_Reset(dev,MOSI)

void SoftSPI_Init(SoftSPI_DeviceTypeDef *dev);
void SoftSPI_HalfDuplex_WriteByte(SoftSPI_DeviceTypeDef *dev,
        unsigned char data);
void SoftSPI_HalfDuplex_WriteData(SoftSPI_DeviceTypeDef *dev,
        unsigned int data, unsigned char size);
void SoftSPI_HalfDuplex_ReadByte(SoftSPI_DeviceTypeDef *dev,
        unsigned char *data);
void SoftSPI_HalfDuplex_ReadData(SoftSPI_DeviceTypeDef *dev,
        unsigned int *data, unsigned char size);

#endif
