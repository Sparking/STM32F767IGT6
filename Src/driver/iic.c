#include "iic.h"

/* IIC 读取SDA的电平 */
#define IIC_ReadSDA(_IIC)                       \
    HAL_GPIO_ReadPin((_IIC)->interface->SDA.GPIO,   \
        (_IIC)->interface->SDA.Pin)
/* IIC SDA引脚写入高低电平 */
#define IIC_WriteSDA(_IIC,_IIC_binary)              \
    HAL_GPIO_WritePin((_IIC)->interface->SDA.GPIO,  \
        (_IIC)->interface->SDA.Pin,         \
        (_IIC_binary) ? GPIO_PIN_SET : GPIO_PIN_RESET)
/* IIC SDA引脚写入高电平 */
#define IIC_SetSDA(_IIC)                    \
    HAL_GPIO_WritePin((_IIC)->interface->SDA.GPIO,  \
        (_IIC)->interface->SDA.Pin, GPIO_PIN_SET)
/* IIC SDA引脚写入低电平 */
#define IIC_ResetSDA(_IIC)                      \
    HAL_GPIO_WritePin((_IIC)->interface->SDA.GPIO,  \
        (_IIC)->interface->SDA.Pin, GPIO_PIN_RESET)
/* IIC SCK引脚拉高 */
#define IIC_SetSCL(_IIC)                    \
    HAL_GPIO_WritePin((_IIC)->interface->SCL.GPIO,  \
        (_IIC)->interface->SCL.Pin, GPIO_PIN_SET)
/* IIC SCK引脚拉低 */
#define IIC_ResetSCL(_IIC)                      \
    HAL_GPIO_WritePin((_IIC)->interface->SCL.GPIO,  \
        (_IIC)->interface->SCL.Pin, GPIO_PIN_RESET)
/* 切换IIC SDA引脚的模式 */
#define IIC_SwitchSDAMode(_IIC, _mode)              \
    GPIO_EasySwitchPinMode(                 \
        (_IIC)->interface->SDA.GPIO,        \
        (_IIC)->interface->SDA.Pin, (_mode))
#define IIC_SDA_MODE_INPUT   GPIO_MODE_INPUT     /* SDA输入模式 */
#define IIC_SDA_MODE_OUT     GPIO_MODE_OUTPUT_PP /* SDA输出模式 */

static const i2c_speed_t i2c_speed[3] = {
    [I2C_100K_SPEED] = {
        .t_hold_start  = 6,     /* 0.6us */
        .t_scl_low     = 13,    /* 1.3us */
        .t_scl_high    = 6,     /* 0.6us */
        .t_setup_start = 6,     /* 0.6us */
        .t_hold_sda    = 0,     /* 0us */
        .t_setup_sda   = 1,     /* 100ns */
        .t_raise       = 3,     /* 300ns */
        .t_fall        = 3,     /* 300ns */
        .t_setup_stop  = 6,     /* 0.6us */
        .t_bus_free    = 13,    /* 1.3us */
        .t_valid_data  = 9,     /* 0.9us */
        .t_vaild_ack   = 9      /* 0.9us */},
    [I2C_400K_SPEED] = {
        /* reference from mpu9250 datasheet */
        .t_hold_start  = 6,     /* 0.6us */
        .t_scl_low     = 13,    /* 1.3us */
        .t_scl_high    = 6,     /* 0.6us */
        .t_setup_start = 6,     /* 0.6us */
        .t_hold_sda    = 0,     /* 0us */
        .t_setup_sda   = 1,     /* 100ns */
        .t_raise       = 3,     /* 300ns */
        .t_fall        = 3,     /* 300ns */
        .t_setup_stop  = 6,     /* 0.6us */
        .t_bus_free    = 13,    /* 1.3us */
        .t_valid_data  = 9,     /* 0.9us */
        .t_vaild_ack   = 9      /* 0.9us */},
    [I2C_1M_SPEED] = {0}
};

__attribute__(( always_inline )) static inline uint16_t i2c_speed2us(
    unsigned char speed)
{
    if (speed % 10) {
        speed += 10;
    }

    return (uint16_t)(speed / 10);
}

/* IIC公用引脚初始化 */
void IIC_Interface_Init(i2c_interface_t *Interface)
{
    GPIO_InitTypeDef GPIO_Initure;
    /* IIC SCL引脚设置 */
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_Initure.Pin  = Interface->SCL.Pin;
    GPIO_Initure.Pull = GPIO_PULLUP;
    GPIO_Initure.Speed= GPIO_SPEED_FAST;
    GPIO_Clk_Enable(Interface->SCL.GPIO);
    HAL_GPIO_Init(Interface->SCL.GPIO, &GPIO_Initure);
    /* IIC SDA引脚设置 */
    GPIO_Initure.Pin = Interface->SDA.Pin;
    GPIO_Clk_Enable(Interface->SDA.GPIO);
    HAL_GPIO_Init(Interface->SDA.GPIO, &GPIO_Initure);
    /* 初始化引脚 */
    GPIO_SetPin(Interface->SCL.GPIO, Interface->SCL.Pin);
    GPIO_SetPin(Interface->SDA.GPIO, Interface->SDA.Pin);
}

void IIC_Init(i2c_device_t *IIC, i2c_interface_t *interface,
        unsigned char addr, unsigned char speed)
{
    IIC->interface = interface;
    IIC->addr = addr;
    IIC->speed = i2c_speed + speed;
}

void IIC_Start(i2c_device_t *IIC)
{
    IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
    IIC_SetSDA(IIC);
    IIC_SetSCL(IIC);
    delayus(4);
    IIC_ResetSDA(IIC);
    delayus(i2c_speed2us(IIC->speed->t_fall
        + IIC->speed->t_hold_start));
    IIC_ResetSCL(IIC);
    delayus(i2c_speed2us(IIC->speed->t_fall));
}

void IIC_Stop(i2c_device_t *IIC)
{
    IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
    IIC_ResetSCL(IIC);
    IIC_ResetSDA(IIC);
    delayus(4);
    IIC_SetSCL(IIC);
    delayus(i2c_speed2us(IIC->speed->t_raise
        + IIC->speed->t_setup_stop));
    IIC_SetSDA(IIC);
    delayus(i2c_speed2us(IIC->speed->t_raise
        + IIC->speed->t_bus_free));
}

unsigned char IIC_WaitAck(i2c_device_t *IIC)
{
    unsigned char ucErrTime = 0U;

    IIC_SetSDA(IIC);
    IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_INPUT);
    delayus(1);
    IIC_SetSCL(IIC);
    delayus(1);
    while (IIC_ReadSDA(IIC) != 0) {
        ucErrTime ++;
        if (ucErrTime > 250U) {
            IIC_Stop(IIC);
            break;
        }
    }
    if (ucErrTime <= 250U) {
        IIC_ResetSCL(IIC);
        ucErrTime = 0U;
    }
    return ucErrTime;
}

void IIC_SendAck(i2c_device_t *IIC, unsigned char ack)
{
    IIC_ResetSCL(IIC);
    IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
    IIC_WriteSDA(IIC, ack);
    delayus(i2c_speed2us(IIC->speed->t_raise)); 
    IIC_SetSCL(IIC);
    delayus(i2c_speed2us(IIC->speed->t_raise + 3));
    IIC_ResetSCL(IIC);
}

void IIC_WriteByte(i2c_device_t *IIC, unsigned char data)
{
    unsigned char i;

    IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
    IIC_ResetSCL(IIC);
    for (i = 0U; i < 8U; i ++) {
        delayus(i2c_speed2us(IIC->speed->t_hold_sda
            + IIC->speed->t_fall + IIC->speed->t_setup_sda));
        IIC_WriteSDA(IIC, data  >> 7);
        delayus(i2c_speed2us(IIC->speed->t_raise));
        IIC_SetSCL(IIC);
        delayus(i2c_speed2us(IIC->speed->t_raise + 3));
        IIC_ResetSCL(IIC);
        delayus(i2c_speed2us(IIC->speed->t_fall));
        data <<= 1;
    }
}

unsigned char IIC_ReadByte(i2c_device_t *IIC, unsigned char ack_signal)
{
    unsigned char i, data;

    IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_INPUT);
    delayus(10);
    IIC_ResetSCL(IIC);
    delayus(i2c_speed2us(IIC->speed->t_fall));
    for (i = 0U, data = 0U; i < 8U; i ++) {
        IIC_SetSCL(IIC);
        delayus(i2c_speed2us(IIC->speed->t_raise));
        data <<= 1U;
        if (IIC_ReadSDA(IIC))
            data++;
        IIC_ResetSCL(IIC);
        delayus(i2c_speed2us(IIC->speed->t_hold_sda
            + IIC->speed->t_fall + IIC->speed->t_setup_sda));
    }
    IIC_SendAck(IIC, ack_signal);
    return data;
}
