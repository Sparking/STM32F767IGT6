ALT_OBJS :=

# 公共库(与平台无关)
LIB_PATH := $(CURDIR)/lib
LIB_OBJS :=
LIB_OBJS += bufchain.o
LIB_OBJS += cli.o
LIB_OBJS += readline.o
LIB_OBJS += bcd2num.o
# 字体库(用于LCD显示)
LIB_OBJS += font.o

CPPFLAGS += -I$(LIB_PATH)/include
ALT_OBJS += $(addprefix $(LIB_PATH)/, $(LIB_OBJS))

# 驱动
BOARD_DRIVER_PATH := $(CURDIR)/driver
DRIVER_OBJS :=
# ADC ad7705驱动(GPIO模拟spi)
DRIVER_OBJS += ad7705_softspi.o
# AT24CXX驱动
DRIVER_OBJS += at24cxx.o
# 延时函数(us级别不精准, 1000us级别是精准的)
DRIVER_OBJS += delay.o
# 温度传感器(GPIO模拟)
DRIVER_OBJS += ds18b20.o
# 外部中断处理
DRIVER_OBJS += exti.o
# GPIO驱动接口
DRIVER_OBJS += gpio.o
# 触控屏驱动
DRIVER_OBJS += gt9147.o
# I2C驱动(GPIO模拟,STM32硬件I2C有缺陷,只能用软件件模拟时序)
DRIVER_OBJS += iic.o
# STM32独立看门狗
DRIVER_OBJS += iwdg.o
# STM32 LTDC(RGB显示屏驱动)
DRIVER_OBJS += lcd.o
DRIVER_OBJS += lcd_touch.o
# 加速度传感器, 尚未实现
# DRIVER_OBJS += mpu9250.o
# OLED驱动, 未实现
# DRIVER_OBJS += oled.o
# 硬件时钟驱动
DRIVER_OBJS += rtc.o
# SDRAM驱动
DRIVER_OBJS += sdram.o
# 软件模拟SPI协议
DRIVER_OBJS += softspi.o
# 硬件SPI
DRIVER_OBJS += spi.o
# 定时器驱动
DRIVER_OBJS += timer.o
# 串口驱动
DRIVER_OBJS += usart.o

CPPFLAGS += -I$(BOARD_DRIVER_PATH)/include
ALT_OBJS += $(addprefix $(BOARD_DRIVER_PATH)/, $(DRIVER_OBJS))

USER_SRC_PATH := $(CURDIR)/src
USER_OBJS += main.o
USER_OBJS += user_gui.o
USER_OBJS += user_lcd_touch.o
USER_OBJS += user_setting_interface.o

CPPFLAGS  += -I$(USER_SRC_PATH)/include
ALT_OBJS  += $(addprefix $(USER_SRC_PATH)/, $(USER_OBJS))

OBJS += $(ALT_OBJS)