#include "iic.h"

/* IIC 读取SDA的电平 */
#define IIC_ReadSDA(_IIC)                                       \
        HAL_GPIO_ReadPin((_IIC)->IIC_Pin_Interface->SDA.GPIO,   \
                (_IIC)->IIC_Pin_Interface->SDA.Pin)
/* IIC SDA引脚写入高低电平 */
#define IIC_WriteSDA(_IIC,_IIC_binary)                          \
        HAL_GPIO_WritePin((_IIC)->IIC_Pin_Interface->SDA.GPIO,  \
                (_IIC)->IIC_Pin_Interface->SDA.Pin,             \
                (_IIC_binary) ? GPIO_PIN_SET : GPIO_PIN_RESET)
/* IIC SDA引脚写入高电平 */
#define IIC_SetSDA(_IIC)                                        \
        HAL_GPIO_WritePin((_IIC)->IIC_Pin_Interface->SDA.GPIO,  \
                (_IIC)->IIC_Pin_Interface->SDA.Pin, GPIO_PIN_SET)
/* IIC SDA引脚写入低电平 */
#define IIC_ResetSDA(_IIC)                                      \
        HAL_GPIO_WritePin((_IIC)->IIC_Pin_Interface->SDA.GPIO,  \
                (_IIC)->IIC_Pin_Interface->SDA.Pin, GPIO_PIN_RESET)
/* IIC SCK引脚拉高 */
#define IIC_SetSCL(_IIC)                                        \
        HAL_GPIO_WritePin((_IIC)->IIC_Pin_Interface->SCL.GPIO,  \
                (_IIC)->IIC_Pin_Interface->SCL.Pin, GPIO_PIN_SET)
/* IIC SCK引脚拉低 */
#define IIC_ResetSCL(_IIC)                                      \
        HAL_GPIO_WritePin((_IIC)->IIC_Pin_Interface->SCL.GPIO,  \
                (_IIC)->IIC_Pin_Interface->SCL.Pin, GPIO_PIN_RESET)
/* 切换IIC SDA引脚的模式 */
#define IIC_SwitchSDAMode(_IIC, _mode)                          \
        GPIO_EasySwitchPinMode(                                 \
                (_IIC)->IIC_Pin_Interface->SDA.GPIO,            \
                (_IIC)->IIC_Pin_Interface->SDA.Pin, (_mode))
#define IIC_SDA_MODE_INPUT   GPIO_MODE_INPUT     /* SDA输入模式 */
#define IIC_SDA_MODE_OUT     GPIO_MODE_OUTPUT_PP /* SDA输出模式 */


/* IIC初始化 */
void IIC_Init(IIC_DeviceTypeDef *IIC)
{
        GPIO_InitTypeDef GPIO_Initure = {0};
/* IIC SCL引脚设置 */
        GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Initure.Pin  = IIC->IIC_Pin_Interface->SCL.Pin;
        GPIO_Initure.Pull = GPIO_PULLUP;
        GPIO_Initure.Speed= GPIO_SPEED_FAST;
        GPIO_Clk_Enable(IIC->IIC_Pin_Interface->SCL.GPIO);
        HAL_GPIO_Init(IIC->IIC_Pin_Interface->SCL.GPIO, &GPIO_Initure);
/* IIC SDA引脚设置 */
        GPIO_Initure.Pin = IIC->IIC_Pin_Interface->SDA.Pin;
        GPIO_Clk_Enable(IIC->IIC_Pin_Interface->SDA.GPIO);
        HAL_GPIO_Init(IIC->IIC_Pin_Interface->SDA.GPIO, &GPIO_Initure);
/* 初始化引脚 */
        IIC_SetSCL(IIC);
        IIC_SetSDA(IIC);
}

/* IIC 发出开始信号 */
void IIC_Start(IIC_DeviceTypeDef *IIC)
{
        IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
        IIC_SetSDA(IIC);
        IIC_SetSCL(IIC);
        delayus(4);
        IIC_ResetSDA(IIC);
        delayus(4);
        IIC_ResetSCL(IIC);

}

/* IIC 发出终止信号 */
void IIC_Stop(IIC_DeviceTypeDef *IIC)
{
        IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
        IIC_ResetSCL(IIC);
        IIC_ResetSDA(IIC);
        delayus(4);
        IIC_SetSCL(IIC);
        IIC_SetSDA(IIC);
        delayus(4);
}

/* IIC 等待应答信号 */
/* 返回值 非0: 接收应答失败
 *          0: 接收应答成功
 */
unsigned char IIC_WaitAck(IIC_DeviceTypeDef *IIC)
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

/* IIC 发送应答信号 */
/* 参数ack
 * 1:发送应答信号 0:不发送应答信号
 */
void IIC_SendAck(IIC_DeviceTypeDef *IIC, unsigned char ack)
{
        IIC_ResetSCL(IIC);
        IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
        IIC_WriteSDA(IIC, ack);
        delayus(2);
        IIC_SetSCL(IIC);
        delayus(2);
        IIC_ResetSCL(IIC);
}

void IIC_WriteByte(IIC_DeviceTypeDef *IIC, unsigned char data)
{
        unsigned char i;

        IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_OUT);
        IIC_ResetSCL(IIC);
        for (i = 0U; i < 8U; i ++) {
                IIC_WriteSDA(IIC, data  >> 7);
                data <<= 1;
                delayus(2);
                IIC_SetSCL(IIC);
                delayus(2);
                IIC_ResetSCL(IIC);
                delayus(2);
        }
}

unsigned char IIC_ReadByte(IIC_DeviceTypeDef *IIC, unsigned char ack_signal)
{
        unsigned char i, data;

        IIC_SwitchSDAMode(IIC, IIC_SDA_MODE_INPUT);
        delayus(10);
        for (i = 0U, data = 0U; i < 8U; i ++) {
                IIC_ResetSCL(IIC);
                delayus(2);
                IIC_SetSCL(IIC);
                data <<= 1U;
                if (IIC_ReadSDA(IIC))
                        data++;
        }
        IIC_SendAck(IIC, ack_signal);
        return data;
}
