#ifndef __MPU9250_H_
#define __MPU9250_H_

#include "iic.h"

/*
 * the I2C address of mpu9250
 *  AD0 = 0, I2C address is 0x68
 *  AD0 = 1, I2C address is 0x69
 */
#define MPU9250_IIC_AD00_ADDR	0x68
#define MPU9250_IIC_AD01_ADDR	0x69


void mpu9250_init(i2c_device_t *dev);
void mpu9250_reset(i2c_device_t *dev);
void mpu9250_config(i2c_device_t *dev);

#endif

