#ifndef __AT24CXX_H_
#define __AT24CXX_H_

#include "iic.h"

#define AT24C01		127
#define AT24C02		255
#define AT24C04		511
#define AT24C08		1023
#define AT24C16		2047
#define AT24C32		4095
#define AT24C64	        8191
#define AT24C128	16383
#define AT24C256	32767
#define EE_TYPE         AT24C02

#define AT24CXX_ADDR    0xA0

#define AT24CXX_Init(dev, interface, nr) \
        IIC_Init((dev), (interface), AT24CXX_ADDR | (nr), I2C_100K_SPEED)
void AT24CXX_WriteByte(i2c_device_t *_at24cxx_dev, unsigned short addr,
        unsigned char data);
void AT24CXX_WriteBuff(i2c_device_t *_at24cxx_dev, unsigned short addr,
        unsigned char *buff, unsigned short len);
void AT24CXX_ReadByte(i2c_device_t *_at24cxx_dev, unsigned short addr,
        unsigned char *pdata);
void AT24CXX_ReadBuff(i2c_device_t *_at24cxx_dev, unsigned short addr,
        unsigned char *buff, unsigned short len);
unsigned char AT24CXX_Check(i2c_device_t *_at24cxx_dev);

#endif
