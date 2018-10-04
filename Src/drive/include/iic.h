#ifndef __IIC_H_
#define __IIC_H_

#include "delay.h"
#include "gpio.h"

typedef Single_GPIOPinTypeDef IIC_GPIOPinTypeDef;

/* I2C操作接口，由于可以多个器件共用相同的接口，因此将接口的公共部分挪出来 */
typedef struct {
        IIC_GPIOPinTypeDef SCL;
	IIC_GPIOPinTypeDef SDA;
} IIC_GPIOInterfaceTypeDef;

/* I2C设备的定义，公共接口+设备地址 */
typedef struct {
        IIC_GPIOInterfaceTypeDef *IIC_Pin_Interface;
        unsigned char IIC_Device_Address;
} IIC_DeviceTypeDef;

/* 初始化I2C设备的公共接口 */
void IIC_Interface_Init(IIC_GPIOInterfaceTypeDef *IIC_Interface);

/* 初始化I2C设备，设置设备的地址 */
__attribute__((always_inline)) static inline void IIC_Init(
		IIC_DeviceTypeDef *IIC, IIC_GPIOInterfaceTypeDef *IIC_Interface,
		unsigned char addr) {
	IIC->IIC_Pin_Interface = IIC_Interface;
	IIC->IIC_Device_Address = addr;
}

/* 开始信号 */
void IIC_Start(IIC_DeviceTypeDef *IIC);

/* 停止信号 */
void IIC_Stop(IIC_DeviceTypeDef *IIC);

/* I2C设备发送应答信号，ack_signal=0表示不发送应答，ack_signal=1表示发送应答 */
void IIC_SendAck(IIC_DeviceTypeDef *IIC, unsigned char ack_signal);

/* 向I2C设备发送一个字节 */
void IIC_WriteByte(IIC_DeviceTypeDef *IIC, unsigned char data);

/* 从I2C被设备读取一个字节 */
unsigned char IIC_ReadByte(IIC_DeviceTypeDef *IIC, unsigned char ack_signal);

/* I2C设备等待应答, 返回0表示等待应答失败，否则应答成功 */
unsigned char IIC_WaitAck(IIC_DeviceTypeDef *IIC);

#endif /* End of __IIC_H_ */

