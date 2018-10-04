#ifndef __MPU9250_H_
#define __MPU9250_H_

#include "iic.h"

/* the I2C address of mpu9250 */
#define MPU9250_IIC_ADDR    0x68

void mpu9250_init(IIC_DeviceTypeDef *dev);
void mpu9250_reset(IIC_DeviceTypeDef *dev);
void mpu9250_config(IIC_DeviceTypeDef *dev);

#endif

