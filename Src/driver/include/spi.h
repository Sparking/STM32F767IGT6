#ifndef __SPI_H_
#define __SPI_H_

#include "gpio.h"
#include "stm32f7xx_hal_spi.h"

extern SPI_HandleTypeDef SPI_Handler;
void SPI_Init(unsigned char baudrate_prescaler);
unsigned char SPI_ReadWriteByte(SPI_HandleTypeDef *shd, unsigned char txdata);
#endif /* __SPI_H_ */
