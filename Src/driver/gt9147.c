#include <string.h>
#include "lcd_touch.h"

static i2c_interface_t gt9147_interface;
static i2c_device_t gt9147_device;

#ifdef SUPPORT_TOUCH_DEVICE_0x5510

const unsigned char GT9147_CFG_TBL[]=
{ 
    0X60, 0XE0, 0X01, 0X20, 0X03, 0X05, 0X35, 0X00, 0X02, 0X08,
    0X1E, 0X08, 0X50, 0X3C, 0X0F, 0X05, 0X00, 0X00, 0XFF, 0X67,
    0X50, 0X00, 0X00, 0X18, 0X1A, 0X1E, 0X14, 0X89, 0X28, 0X0A,
    0X30, 0X2E, 0XBB, 0X0A, 0X03, 0X00, 0X00, 0X02, 0X33, 0X1D,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X32, 0X00, 0X00,
    0X2A, 0X1C, 0X5A, 0X94, 0XC5, 0X02, 0X07, 0X00, 0X00, 0X00,
    0XB5, 0X1F, 0X00, 0X90, 0X28, 0X00, 0X77, 0X32, 0X00, 0X62,
    0X3F, 0X00, 0X52, 0X50, 0X00, 0X52, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X0F,
    0X0F, 0X03, 0X06, 0X10, 0X42, 0XF8, 0X0F, 0X14, 0X00, 0X00,
    0X00, 0X00, 0X1A, 0X18, 0X16, 0X14, 0X12, 0X10, 0X0E, 0X0C,
    0X0A, 0X08, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0X00, 0X29, 0X28, 0X24, 0X22, 0X20, 0X1F, 0X1E, 0X1D,
    0X0E, 0X0C, 0X0A, 0X08, 0X06, 0X05, 0X04, 0X02, 0X00, 0XFF,
    0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00, 0X00,
    0X00, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF, 0XFF,
    0XFF, 0XFF, 0XFF, 0XFF,
};
 
/*
 * 发送GT9147配置参数
 * mode:0,参数不保存到flash
 *    1,参数保存到flash
 */
unsigned char GT9147_Send_Cfg(i2c_device_t *dev, unsigned char mode)
{
    unsigned char buf[2];
    unsigned char i = 0;

    buf[0] = 0;
    /* 是否掉电保存 */
    buf[1] = mode;
    /* 计算校验和 */
    for (i = 0; i < sizeof(GT9147_CFG_TBL); i++)
        buf[0] += GT9147_CFG_TBL[i];
    buf[0] = (~buf[0]) + 1;
    /* 发送寄存器配置 */
    GT9147_WR_Reg(dev, GT_CFGS_REG, (unsigned char*)GT9147_CFG_TBL,
            sizeof(GT9147_CFG_TBL));
    /* 写入校验和,和配置更新标记 */
    GT9147_WR_Reg(dev, GT_CHECK_REG, buf, 2);

    return 0;
}

#endif

/*
 * 向GT9147写入一次数据
 * reg:起始寄存器地址
 * buf:数据缓缓存区
 * len:写数据长度
 * 返回值:0,成功;1,失败.
 */
unsigned char GT9147_WR_Reg(i2c_device_t *dev, unsigned int reg,
                            unsigned char *buf, unsigned char len)
{
    unsigned char i;
    unsigned char ret = 0;

    IIC_Start(dev);    
    IIC_WriteByte(dev, dev->addr); /* IIC Write */ 
    IIC_WaitAck(dev);
    IIC_WriteByte(dev, reg >> 8);
    IIC_WaitAck(dev);                                                           
    IIC_WriteByte(dev, reg & 0XFF);
    IIC_WaitAck(dev);
    for (i = 0; i < len && !ret; i++) {
        IIC_WriteByte(dev, buf[i]);
        ret = IIC_WaitAck(dev);
    }
    IIC_Stop(dev);

    return ret; 
}

/*
 * 从GT9147读出一次数据
 * reg:起始寄存器地址
 * buf:数据缓缓存区
 * len:读数据长度
 */
void GT9147_RD_Reg(i2c_device_t *dev, unsigned int reg,
                   unsigned char *buf, unsigned char len)
{
    unsigned char i;
    IIC_Start(dev);
    IIC_WriteByte(dev, dev->addr);
    IIC_WaitAck(dev);
    IIC_WriteByte(dev, reg >> 8);
    IIC_WaitAck(dev);
    IIC_WriteByte(dev, reg & 0XFF);
    IIC_WaitAck(dev);
    IIC_Start(dev);
    IIC_WriteByte(dev, dev->addr + 1);
    IIC_WaitAck(dev);
    for (i = 0; i < len; i++)
        buf[i] = IIC_ReadByte(dev, i == len - 1);
    IIC_Stop(dev);
}

/* 初始化GT9147触摸屏 */
unsigned char GT9147_Init(void)
{
    unsigned char temp[5];
    GPIO_InitTypeDef GPIO_Initure;
    i2c_device_t *dev = &gt9147_device;

    gt9147_interface.SCL.Pin  = GPIO_PIN_6;
    gt9147_interface.SCL.GPIO = GPIOH;
    gt9147_interface.SDA.Pin  = GPIO_PIN_3;
    gt9147_interface.SDA.GPIO = GPIOI;

    GPIO_Initure.Pin   = GPIO_PIN_7;
    GPIO_Initure.Mode  = GPIO_MODE_INPUT;
    GPIO_Initure.Pull  = GPIO_PULLUP;
    GPIO_Initure.Speed = GPIO_SPEED_HIGH;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);

    GPIO_Initure.Pin  = GPIO_PIN_8;
    GPIO_Initure.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOI, &GPIO_Initure);

    /* 初始化电容屏的IIC总线 */
    IIC_Interface_Init(&gt9147_interface);
    IIC_Init(dev, &gt9147_interface, GT9147_ADDR, I2C_100K_SPEED);
    GT_RST_Reset();
    delayms(10);
    GT_RST_Set();
    delayms(10); 

    GPIO_Initure.Pin   = GPIO_PIN_7;
    GPIO_Initure.Mode  = GPIO_MODE_INPUT;
    GPIO_Initure.Pull  = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOH, &GPIO_Initure);
    delayms(100);
    GT9147_RD_Reg(dev, GT_PID_REG, temp, 4); /* 读取PID寄存器 */
    temp[4] = '\0';
    printf("CTP PID:%s\r\n", temp);
    temp[0] = 0X02;
    GT9147_WR_Reg(dev, GT_CTRL_REG, temp, 1); /* 软复位GT9147 */
    GT9147_RD_Reg(dev, GT_CFGS_REG, temp, 1); /* 读取GT_CFGS_REG寄存器 */
    /* 默认版本比较低,需要更新flash配置 */
    if (temp[0] < 0X60) {
        printf("Default Ver:%d\r\n", temp[0]);
#ifdef SUPPORT_TOUCH_DEVICE_0x5510
        GT9147_Send_Cfg(dev, 1); 
#endif
    }
    delayms(10);
    temp[0] = 0X00;     
    GT9147_WR_Reg(dev, GT_CTRL_REG, temp, 1); /* 结束复位 */

    return 0;
}

const unsigned int GT9147_TPX_TBL[5] = {
    GT_TP1_REG, GT_TP2_REG, GT_TP3_REG, GT_TP4_REG, GT_TP5_REG};
/*
 * 扫描触摸屏(采用查询方式)
 * mode:0,正常扫描.
 * 返回值:当前触屏状态.
 * 0,触屏无触摸;1,触屏有触摸
 */
unsigned char GT9147_Scan()
{
    /* 控制查询间隔,从而降低CPU占用率 */
    static unsigned char t = 1;

    unsigned char i;
    unsigned char mode;
    unsigned char temp;
    unsigned char tempsta;
    unsigned char buf[4];
    unsigned char res = 0;
    i2c_device_t *dev = &gt9147_device;

    /* 空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率 */
    if ((t % 10) == 0 || t < 10) {
        /* 读取触摸点的状态 */
        GT9147_RD_Reg(dev, GT_GSTID_REG, &mode, 1);
         if ((mode & 0X80) && ((mode & 0XF) < 6)) {
            temp = 0;
            /* 清标志 */
            GT9147_WR_Reg(dev, GT_GSTID_REG, &temp, 1);
        }        
        if ((mode & 0X0F) && ((mode & 0XF) < 6)) {
            /* 将点的个数转换为1的位数,匹配__lcd_tp_device.sta定义 */
            temp = 0XFF << (mode & 0XF);
            /* 保存当前的__lcd_tp_device.sta值 */
            tempsta = __lcd_tp_device.sta;
            __lcd_tp_device.sta = (~temp) | TP_PRES_DOWN | TP_CATH_PRES;
            /* 保存触点0的数据 */
            __lcd_tp_device.x[4] = __lcd_tp_device.x[0];
            __lcd_tp_device.y[4] = __lcd_tp_device.y[0];
            for (i = 0; i < CT_MAX_TOUCH; i++) {
                /* 触摸有效? */
                if (__lcd_tp_device.sta & (1 << i)) {
                    GT9147_RD_Reg(dev, GT9147_TPX_TBL[i], buf, 4); /* 读取XY坐标值 */
                    if (__lcd_tp_device.dir == LCD_DIRECTION_H) {
                            __lcd_tp_device.x[i] = (((unsigned int)buf[1] << 8) + buf[0]);
                            __lcd_tp_device.y[i] = (((unsigned int)buf[3] << 8) + buf[2]);
                    }
                    else {
                            __lcd_tp_device.y[i] = ((unsigned int)buf[1] << 8) + buf[0];
                            __lcd_tp_device.x[i] = __ltdc_lcd_device.active_heigh -
                                    (((unsigned int)buf[3] << 8) + buf[2]);
                    }
                }
            }
            res = 1;
            /* 非法数据(坐标超出了) */
            if (__lcd_tp_device.x[0] > __ltdc_lcd_device.active_heigh ||
                    __lcd_tp_device.y[0] > __ltdc_lcd_device.active_width) {
                /* 有其他点有数据,则复第二个触点的数据到第一个触点. */
                if ((mode & 0XF) > 1) {
                    /* 触发一次,则会最少连续监测10次,从而提高命中率 */
                    __lcd_tp_device.x[0] = __lcd_tp_device.x[1];
                    __lcd_tp_device.y[0] = __lcd_tp_device.y[1];
                    t = 0;
                }
                else {
                    /* 非法数据,则忽略此次数据(还原原来的) */
                    __lcd_tp_device.x[0] = __lcd_tp_device.x[4];
                    __lcd_tp_device.y[0] = __lcd_tp_device.y[4];
                    mode = 0X80;
                    /* 恢复__lcd_tp_device.sta */
                    __lcd_tp_device.sta = tempsta;
                }
            }
            else
                t = 0;
        }
    }
        /* 无触摸点按下 */
    if ((mode & 0X8F) == 0X80) {
        /* 之前是被按下的 */
        if(__lcd_tp_device.sta & TP_PRES_DOWN)
            __lcd_tp_device.sta &= ~(1 << 7); /* 标记按键松开 */
        else { /* 之前就没有被按下 */
            __lcd_tp_device.x[0] = 0xffff;
            __lcd_tp_device.y[0] = 0xffff;
            __lcd_tp_device.sta &= 0XE0; /* 清除点有效标记 */
        }     
    }
    if (t > 240)
        t = 10;

    return res;
}
