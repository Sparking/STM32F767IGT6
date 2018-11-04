#ifndef __IIC_H_
#define __IIC_H_

#include "delay.h"
#include "gpio.h"

typedef Single_GPIOPinTypeDef i2c_pin_t;

/* I2C操作接口，由于可以多个器件共用相同的接口，因此将接口的公共部分挪出来 */
typedef struct {
    i2c_pin_t SCL;
    i2c_pin_t SDA;
} i2c_interface_t;

enum {
    I2C_100K_SPEED = 0,
    I2C_400K_SPEED,
    I2C_1M_SPEED,
};

typedef struct {
    /* 单位 x100ns */
    unsigned char t_hold_start; /* 总线开始时间 */
    unsigned char t_scl_low;    /* SCL低电平时长 */
    unsigned char t_scl_high;   /* SCL高电平时长 */
    unsigned char t_setup_start;/* 开始设置时间 */
    unsigned char t_hold_sda;   /* 数据保持时长 */
    unsigned char t_setup_sda;  /* 数据设置时长 */
    unsigned char t_raise;      /* SCL, SDA上升沿时间 */
    unsigned char t_fall;       /* SCL, SDA下降沿时间 */
    unsigned char t_setup_stop; /* 停止设置时间 */
    unsigned char t_bus_free;   /* 总线空余时间 */
    unsigned char t_valid_data; /* 数据有效期 */
    unsigned char t_vaild_ack;  /* 数据确认时间 */
} i2c_speed_t;

/* I2C设备的定义，公共接口+设备地址 */
typedef struct {
    i2c_interface_t *interface;
    const i2c_speed_t *speed;
    unsigned char addr;
} i2c_device_t;

/* 初始化I2C设备的公共接口 */
void IIC_Interface_Init(i2c_interface_t *interface);

/* 初始化I2C设备，设置设备的地址，通信速率 */
void IIC_Init(i2c_device_t *IIC, i2c_interface_t *interface,
        unsigned char addr, unsigned char speed);

/* 开始信号 */
void IIC_Start(i2c_device_t *IIC);

/* 停止信号 */
void IIC_Stop(i2c_device_t *IIC);

/* I2C设备发送应答信号，ack_signal=0表示不发送应答，ack_signal=1表示发送应答 */
void IIC_SendAck(i2c_device_t *IIC, unsigned char ack_signal);

/* 向I2C设备发送一个字节 */
void IIC_WriteByte(i2c_device_t *IIC, unsigned char data);

/* 从I2C被设备读取一个字节 */
unsigned char IIC_ReadByte(i2c_device_t *IIC, unsigned char ack_signal);

/* I2C设备等待应答, 返回0表示等待应答失败，否则应答成功 */
unsigned char IIC_WaitAck(i2c_device_t *IIC);

#endif /* End of __IIC_H_ */

