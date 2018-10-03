#ifndef __GT9147_CONFIG_H_
#define __GT9147_CONFIG_H_

#include "iic.h"

/* IO操作 */
#define GT_RST_Reset()  HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_RESET) /* GT9147复位引脚 */
#define GT_RST_Set()    HAL_GPIO_WritePin(GPIOI, GPIO_PIN_8, GPIO_PIN_SET)
#define GT_INT          HAL_GPIO_ReadPin(GPIOH, GPIO_PIN_7)  /* GT9147中断引脚 */
 
/* GT9147的IIC地址 */
#define GT9147_ADDR             0x28
  
/* GT9147 */
#define GT_CTRL_REG             0X8040          /* GT9147控制寄存器 */
#define GT_CFGS_REG             0X8047          /* GT9147配置起始地址寄存器 */
#define GT_CHECK_REG            0X80FF          /* GT9147校验和寄存器 */
#define GT_PID_REG              0X8140          /* GT9147产品ID寄存器 */
#define GT_GSTID_REG            0X814E          /* GT9147当前检测到的触摸情况 */
#define GT_TP1_REG              0X8150          /* 第一个触摸点数据地址 */
#define GT_TP2_REG              0X8158          /* 第二个触摸点数据地址 */
#define GT_TP3_REG              0X8160          /* 第三个触摸点数据地址 */
#define GT_TP4_REG              0X8168          /* 第四个触摸点数据地址 */
#define GT_TP5_REG              0X8170          /* 第五个触摸点数据地址 */

unsigned char GT9147_Init(void);
unsigned char GT9147_Scan(void);

void GT9147_RD_Reg(IIC_DeviceTypeDef *dev, unsigned int reg,
                   unsigned char *buf, unsigned char len);
unsigned char GT9147_WR_Reg(IIC_DeviceTypeDef *dev, unsigned int reg,
                            unsigned char *buf, unsigned char len);
unsigned char GT9147_Send_Cfg(IIC_DeviceTypeDef *dev, unsigned char mode);
#endif
