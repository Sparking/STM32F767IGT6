#include "spi.h"

SPI_HandleTypeDef SPI_Handler;

void SPI_Init(unsigned char baudrate_prescaler)
{
        SPI_Handler.Instance = SPI2;
        SPI_Handler.Init.Mode = SPI_MODE_MASTER; /* 设置SPI工作模式，设置为主模式 */
        SPI_Handler.Init.Direction = SPI_DIRECTION_2LINES; /* 设置SPI单向或者双向的数据模式:SPI设置为双线模式 */
        SPI_Handler.Init.DataSize = SPI_DATASIZE_8BIT; /* 设置SPI的数据大小:SPI发送接收8位帧结构 */
        SPI_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH; /* 串行同步时钟的空闲状态为高电平 */
        SPI_Handler.Init.CLKPhase = SPI_PHASE_2EDGE; /* 串行同步时钟的第二个跳变沿（上升或下降）数据被采样 */
        SPI_Handler.Init.NSS = SPI_NSS_SOFT; /* NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制 */
        SPI_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256; /* 定义波特率预分频的值:波特率预分频值为256 */
        SPI_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB; /* 指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始 */
        SPI_Handler.Init.TIMode = SPI_TIMODE_DISABLE; /* 关闭TI模式 */
        SPI_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE; /* 关闭硬件CRC校验 */
        SPI_Handler.Init.CRCPolynomial = 7; /* CRC值计算的多项式 */
        SPI_Handler.Instance->CR1 &= 0XFFC7; /* 位3-5清零，用来设置波特率 */
        SPI_Handler.Instance->CR1 |= baudrate_prescaler; /* 设置SPI速度 */
        __HAL_RCC_SPI2_CLK_ENABLE();
        HAL_SPI_Init(&SPI_Handler);
        __HAL_SPI_ENABLE(&SPI_Handler);
        SPI_ReadWriteByte(&SPI_Handler, 0Xff); /* 启动传输 */
}

unsigned char SPI_ReadWriteByte(SPI_HandleTypeDef *shd, unsigned char txdata)
{
        unsigned char rxdata;
        HAL_SPI_TransmitReceive(shd, &txdata, &rxdata, 1, 1000);
        return rxdata;
}
