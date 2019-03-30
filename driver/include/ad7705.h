#ifndef _DRIVE_AD7705_H_
#define _DRIVE_AD7705_H_

#include "softspi.h"

#define AD7705_SCLK_GPIO GPIOB
#define AD7705_SCLK_PIN  GPIO_PIN_13
#define AD7705_DOUT_GPIO GPIOB
#define AD7705_DOUT_PIN  GPIO_PIN_14
#define AD7705_DIN_GPIO  GPIOB
#define AD7705_DIN_PIN   GPIO_PIN_15
/*
#define AD7705_CS_GPIO   GPIOA
#define AD7705_CS_PIN    GPIO_PIN_4
#define AD7705_RST_GPIO  GPIOA
#define AD7705_RST_PIN   GPIO_PIN_8
#define AD7705_DRDY_GPIO GPIOG
#define AD7705_DRDY_PIN  GPIO_PIN_10
#define AD7705_DRDY_EXTI_IRQ EXTI15_10_IRQn
*/

/* AD7705寄存器(寄存器无特殊说明均为8位), 共有8个*/
#define AD7705_REG(regn)        (regn << 4U)
#define AD7705_REG_CMN          AD7705_REG(0) /* 通信寄存器 */
#define AD7705_REG_CFG          AD7705_REG(1) /* 设置寄存器 */
#define AD7705_REG_CLK          AD7705_REG(2) /* 时钟寄存器 */
#define AD7705_REG_DAT          AD7705_REG(3) /* 数据寄存器(16位) */
#define AD7705_REG_TST          AD7705_REG(4) /* 测试寄存器 */
#define AD7705_REG_NOP          AD7705_REG(5) /* 无操作 */
#define AD7705_REG_OFFSET       AD7705_REG(6) /* 偏移寄存器(24位) */
#define AD7705_REG_GAIN         AD7705_REG(7) /* 增益寄存器(24位) */

/* 通信寄存器,寄存器0。所有与器件的通信必须从写该寄存器开始 */
/* | 0/DRDY | RS2(0) | RS1(0) | RS0(0) | R/W(0) | STBY(0) | CH1(0) | CH0(0) | */
#define AD7705_DRAY_STATUS      0x80U /* 等同于DRAY引脚的状态 */
#define AD7705_CMD_WRITE        0x00U /* 写操作 */
#define AD7705_CMD_READ         0x08U /* 读操作 */
#define AD7705_CMD_STBY         0x04U /* 等待模式 */
/* 校准寄存器对,AIN(+) AIN(-) */
#define AD7705_CALIBRATION_REGS_0    0x00U /* 寄存器对0,AIN1(+) AIN1(-) */ 
#define AD7705_CALIBRATION_REGS_1    0x01U /* 寄存器对1,AIN2(+) AIN2(-) */ 
#define AD7705_CALIBRATION_REGS_2    0x02U /* 寄存器对0,AIN1(-) AIN1(-) */ 
#define AD7705_CALIBRATION_REGS_3    0x03U /* 寄存器对2,AIN1(-) AIN2(-) */

/* 设置寄存器 */
/* | MD1(0) | MD0(0) | G2(0) | G1(0) | G0(0) | B/U(0) | BUF(0) | FSTNC(1) | */
/* 工作模式设定 */
#define AD7705_NORMAL_MODE           0x00U /* 正常工作 */
#define AD7705_SELF_CALIBRATION      0x40U /* 自校准 */
#define AD7705_SYS_ZERO_CALIBRATION  0x80U /* 零标度系统校准 */
#define AD7705_SYS_FULL_CALIBRATION  0xC0U /* 满标度系统校准 */
/* 增益设定 */
#define AD7705_GAIN_1           0x00U /* 1 */
#define AD7705_GAIN_2           0x08U /* 2 */
#define AD7705_GAIN_4           0x10U /* 4 */
#define AD7705_GAIN_8           0x18U /* 8 */
#define AD7705_GAIN_16          0x20U /* 16 */
#define AD7705_GAIN_32          0x28U /* 32 */
#define AD7705_GAIN_64          0x30U /* 64 */
#define AD7705_GAIN_128         0x38U /* 128 */
/* AD7705输入电压的极性 */
#define AD7705_UNIPOLAR         0x04U /* 单极性 */
#define AD7705_BIPOLAR          0x00U /* 双极性 */
/* 缓冲器控制 */
#define AD7705_BUFF_OPEN        0x02U /* 开启缓冲电路 */
#define AD7705_BUFF_CLOSE       0x00U /* 短路缓冲电路，输入端允许处理高阻抗源 */
/*
 * 滤波器同步 该位处于高电平时,数字滤波器的节点、滤波器控制逻辑和校准控制逻辑
 * 处于复位状态下,同时,模拟调制器也被控制在复位状态下。当处于低电平时,调制器
 * 和滤波器开始处理数据,并在 3×(1/输出更新速率)时间内(也就是滤器的稳定时间)产
 * 生一个有效字。FSYNC 不影响数字接口,也不使 DRDY输出复位(如果它是低电平)。
 */
#define AD7705_FILTER_SYNC_RESET 0x01U
#define AD7705_FILTER_SYNC_SET   0x00U

/* 时钟寄存器 */
/* | ZERO(0) | ZERO(0) | ZERO(0) | CLKDIS(0) | CLKDIV(0) | CLK(1) | FSI(0) | FS0(1) | */
/* 禁止主时钟在MCLKOUT引脚输出，禁止时MCLKOUT为低电平，可以省电 */
#define AD7705_DISABLE_MCLK      0x10U
#define AD7705_ENABLE_MCLK       0x00U /* 开启主时钟 */
/* 时钟分频系数 */
#define AD7705_CLKDIV_0          0x00U /* 不分频 */
#define AD7005_CLKDIV_2          0x08U /* 2分频 */
/* 时钟源选择 */
/* 使用1MHz(CLKDIV=0)或2MHz(CLKDIV=1)的晶振 */
#define AD7705_CLKSRC_0          0x04U
/* 使用2.4576MHz(CLKDIV=0)或4.9152MHz(CLKDIV=1)的晶振 */
#define AD7705_CLKSRC_1          0x00U
/* 滤波选择位 */
/* 0~3,具体信息查看AD7705中文数据手册表12 输出更新速率 */
#define AD7705_FILTER_SELECT(n)  (n)

void AD7705_SoftSPI_Register(SoftSPI_DeviceTypeDef *dev);
void AD7705_SoftSPI_Init(SoftSPI_DeviceTypeDef *dev);
void AD7705_SoftSPI_ConfigCLK(SoftSPI_DeviceTypeDef *dev);
void AD7705_SoftSPI_Calibration(SoftSPI_DeviceTypeDef *dev);
unsigned short AD7705_SoftSPI_ReadData(SoftSPI_DeviceTypeDef *dev);

#endif /* End of _DRIVE_AD7705_H_ */
