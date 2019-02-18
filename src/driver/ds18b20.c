#include "ds18b20.h"

#define DS18B20_ReadDQ(_ds18b20) \
        HAL_GPIO_ReadPin((_ds18b20)->GPIO, (_ds18b20)->Pin)
#define DS18B20_SetDQ(_ds18b20) \
        HAL_GPIO_WritePin((_ds18b20)->GPIO, (_ds18b20)->Pin, GPIO_PIN_SET)
#define DS18B20_WriteDQ(_ds18b20,_bin) \
        HAL_GPIO_WritePin((_ds18b20)->GPIO, (_ds18b20)->Pin, (_bin) ? GPIO_PIN_SET : GPIO_PIN_RESET)
#define DS18B20_ResetDQ(_ds18b20) \
        HAL_GPIO_WritePin((_ds18b20)->GPIO, (_ds18b20)->Pin, GPIO_PIN_RESET)
/* mode：GPIO_MODE_INPUT GPIO_MODE_OUTPUT_PP */
#define DS18B20_SwitchDQMode(_ds18b20,mode) \
        GPIO_EasySwitchPinMode((_ds18b20)->GPIO, (_ds18b20)->Pin, mode)

/**
 * DS18B20_Register
 * DS18B20设备注册函数
 */
void DS18B20_Register(Single_GPIOPinTypeDef *dev, GPIO_TypeDef *GPIO,
        unsigned int Pin)
{
        GPIO_InitTypeDef GPIO_Init;

        dev->GPIO = GPIO;
        dev->Pin = Pin;
        GPIO_Init.Pin = Pin;
        GPIO_Init.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_Init.Pull = GPIO_PULLUP;
        GPIO_Init.Speed = GPIO_SPEED_FAST;
        GPIO_Clk_Enable(GPIO);
        HAL_GPIO_Init(GPIO, &GPIO_Init);
        DS18B20_SetDQ(dev);
}

extern void IWDG_Feed(void);
unsigned char DS18B20_Check(Single_GPIOPinTypeDef *dev)
{
        unsigned char retry, status;

        retry = 0;
        status = 1;
        IWDG_Feed();
        DS18B20_SwitchDQMode(dev, GPIO_MODE_INPUT);
        while (DS18B20_ReadDQ(dev) && retry < 100) {
                retry++;
                delayus(1);
        }
        if (retry >= 100) {
                status = 0;
        }
        else {
                retry = 0;
                while (!DS18B20_ReadDQ(dev) && retry < 120) {
                        retry++;
                        delayus(1);
                }
                if (retry >= 120)
                        status = 0;
        }

        return status;
}

/**
 * DS18B20_Reset
 * @dev: the ds18b20 device pointer
 * 
 * return 0 if reset failed, but return 1 while reset successed.
 */
unsigned char DS18B20_Reset(Single_GPIOPinTypeDef *dev)
{
        DS18B20_SwitchDQMode(dev, GPIO_MODE_OUTPUT_PP);

        DS18B20_SetDQ(dev);
        delayus(16);
        DS18B20_ResetDQ(dev);
        delayus(280); /* 总线拉低480us */
        DS18B20_SetDQ(dev); /* 主机释放总线 */
        delayus(15);
        /* 延时之后读出低电平时间，如果时间不是60us-240us则认为初始化失败 */
        return DS18B20_Check(dev);
}

void DS18B20_WriteByte(Single_GPIOPinTypeDef *dev, unsigned char byte)
{
        unsigned char index;

        DS18B20_SwitchDQMode(dev, GPIO_MODE_OUTPUT_PP);
        for (index = 0; index < 8; index++) {
                DS18B20_ResetDQ(dev);
                delayus(7);
                if (byte & 0x01U)
                        DS18B20_SetDQ(dev);
                byte >>= 1;
                delayus(23);
                DS18B20_SetDQ(dev);
                delayus(1);
        }
}

unsigned char DS18B20_ReadByte(Single_GPIOPinTypeDef *dev)
{
        unsigned char index;
        unsigned char byte;

        for (index = 0, byte = 0; index < 8; index++) {
                DS18B20_SwitchDQMode(dev, GPIO_MODE_OUTPUT_PP);
                DS18B20_SetDQ(dev);
                delayus(1);
                DS18B20_ResetDQ(dev);
                DS18B20_SwitchDQMode(dev, GPIO_MODE_INPUT);
                delayus(7);
                byte >>= 1;
                if (DS18B20_ReadDQ(dev))
                        byte |= 0x80U;
                DS18B20_SwitchDQMode(dev, GPIO_MODE_OUTPUT_PP);
                DS18B20_SetDQ(dev);
                delayus(23);
        }
        return byte;
}

unsigned char DS18B20_Start(Single_GPIOPinTypeDef *dev)
{
        unsigned char ret = 0U;

        if (DS18B20_Reset(dev)) {
                delayms(1);
                DS18B20_WriteByte(dev, 0xCCU);
                DS18B20_WriteByte(dev, 0x44U);
                ret = 1U;
        }
        return ret;
}

float DS18B20_GetTemp(Single_GPIOPinTypeDef *dev)
{
        float temp;
        unsigned char TL, TH;

        DS18B20_Start(dev);
        delayms(94);
        IWDG_Feed();
        if (DS18B20_Reset(dev) == 0)
                return 0xFFFF;
        DS18B20_WriteByte(dev, 0xCCU);
        DS18B20_WriteByte(dev, 0xBEU);
        TL = DS18B20_ReadByte(dev); 
        TH = DS18B20_ReadByte(dev);
        if (TH & 0xF8U) {
                TH = ~TH;
                TL = ~TL; 
                temp = -1.0f;
        }
        else
                temp = 1.0F;
        temp *= (float)((TH << 8) + TL) * 0.0625;

        return temp;
}
