#ifndef __IIC_CONFIG_H_
#define __IIC_CONFIG_H_

#include "delay.h"
#include "gpio.h"

typedef Single_GPIOPinTypeDef IIC_GPIOPinTypeDef;

typedef struct {
        IIC_GPIOPinTypeDef SCL, SDA;
} IIC_GPIOInterfaceTypeDef;

typedef struct {
        IIC_GPIOInterfaceTypeDef *IIC_Pin_Interface;
        unsigned char IIC_Device_Address;
} IIC_DeviceTypeDef;

void IIC_Init(IIC_DeviceTypeDef *IIC);
void IIC_Start(IIC_DeviceTypeDef *IIC);
void IIC_Stop(IIC_DeviceTypeDef *IIC);
void IIC_SendAck(IIC_DeviceTypeDef *IIC, unsigned char ack_signal);
void IIC_WriteByte(IIC_DeviceTypeDef *IIC, unsigned char data);
unsigned char IIC_ReadByte(IIC_DeviceTypeDef *IIC, unsigned char ack_signal);
unsigned char IIC_WaitAck(IIC_DeviceTypeDef *IIC);

#endif
