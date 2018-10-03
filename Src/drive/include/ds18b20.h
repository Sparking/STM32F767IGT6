#ifndef __DS18B20_H__
#define __DS18B20_H__

#include "delay.h"
#include "gpio.h"

#define  DS18B20_Init(dev) DS18B20_Reset(dev)

void DS18B20_Register(Single_GPIOPinTypeDef *dev, GPIO_TypeDef *GPIO,
        unsigned int Pin);
unsigned char DS18B20_Reset(Single_GPIOPinTypeDef *dev);
unsigned char DS18B20_Start(Single_GPIOPinTypeDef *dev);
unsigned char DS18B20_ReadByte(Single_GPIOPinTypeDef *dev);
void DS18B20_WriteByte(Single_GPIOPinTypeDef *dev, unsigned char byte);
float DS18B20_GetTemp(Single_GPIOPinTypeDef *dev);


#endif /* __DS18B20_H__ */
