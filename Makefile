#Directory

OS       := $(shell uname)
DIR_DIV  := /
MKDIR    := mkdir -p
RMDIR    := rmdir
RM       := rm -f
print    := printf

# 彩色输出
color_red    := \\033[31m
color_green  := \\033[32m
color_yellow := \\033[33m
close_color  := \\033[0m
bol          := \\r\\033[K
eol          := \\r\\n

ROOTDIR   = $(CURDIR)

# 输出的二进制文件的存放位置
O       ?= $(ROOTDIR)/Build
OBJDIR  ?= $(O)/GCC_Objects

# 输出的文件名
MCU      = stm32f767igt6
# .bin文件是直接烧录到设备中的二进制文件
BINARY   := $(O)/$(MCU).bin
# .elf是编译器输出的可执行文件, 文件格式为elf
ELF      := $(O)/$(MCU).elf
# .hex文件
HEX      := $(O)/$(MCU).hex

# STM32F767的CPU内核类型
CORE     := cortex-m7
FPUABI   := hard
FPUTYPE  := fpv5-d16

# 交叉编译工具链
CROSS_COMPILE = arm-none-eabi-
CC       := $(CROSS_COMPILE)gcc
AS       := $(CROSS_COMPILE)as
LD       := $(CROSS_COMPILE)ld
SIZE     := $(CROSS_COMPILE)size
OBJCOPY  := $(CROSS_COMPILE)objcopy


# 调式开关
RELEASE = debug

ifeq ($(RELEASE), release)
# 发行版本, 优化代码大小
CODE_OPT_FLAGS := -Os
DEBUG_FLAGS    :=
else ifeq ($(RELEASE), debug)
# 调试版本, 只用于开发(防止代码优化, 可用于gdb调试)
CODE_OPT_FLAGS := -O0
DEBUG_FLAGS    := -gdwarf-2
endif

# 编译参数
TARGET_FLAGS   := -mthumb -mcpu=$(CORE)
FPUFLAGS       := -mfloat-abi=$(FPUABI) -mfpu=$(FPUTYPE)
# -u _printf_float, 将允许printf打印浮点数, 但是这将导致代码体积变大
CPPFLAGS       += -u _printf_float
ASFLAGS        := $(TARGET_FLAGS) $(DEBUG_FLAGS) $(FPUFLAGS)
# 编译期间使用的specs, nano表示使用newlib, nosys表示无操作系统
SPECS_FLAGS    := --specs=nano.specs --specs=nosys.specs
WARNING_RANK   := -Wall -Werror
# 代码优化选项, 链接时可以去掉没有使用的函数块和数据块
CODE_OPT_FLAGS += -ffunction-sections -fdata-sections

CPPFLAGS := -std=gnu99 \
    $(WARNING_RANK) $(DEBUG_FLAGS) $(CODE_OPT_FLAGS) \
    $(TARGET_FLAGS) $(FPUFLAGS) $(SPECS_FLAGS)

# STM32F767使用CMSIS库需要开启的宏
CMSIS_CPPFLAGS :=    \
    -DSTM32F767xx    \
    -DARM_MATH_CM7   \
    -D__FPU_PRESENT  \
    -D__FPU_USED=1U  \
    -DUNALIGNED_SUPPORT_DISABLE

# 使用HAL库
HAL_CPPFLAGS := -DUSE_HAL_DRIVER

CPPFLAGS += \
	$(CMSIS_CPPFLAGS) \
	$(HAL_CPPFLAGS)
LDFLAGS := \
    -Wl,-gc-section   \
    $(CODE_OPT_FLAGS) \
    $(DEBUG_OPT)      \
    $(TARGET_FLAGS)   \
    $(FPUFLAGS)       \
    $(SPECS_FLAGS)

LIBS :=

SYS_LOCATION_PATH := $(ROOTDIR)/sys
# 官方提供的库
CONTRIB_PATH      := $(SYS_LOCATION_PATH)/contrib

# CMSIS
CMSIS_PATH        := $(CONTRIB_PATH)/cmsis
CMSIS_LIB_PATH    := $(CMSIS_PATH)/lib
CPPFLAGS          += -I$(CMSIS_PATH)
LDFLAGS           += -L$(CMSIS_LIB_PATH)
# CMSIS有提供一个数学库, 如果没事使用到数学库提供的接口, 可以删除这个
LIBS              += -lm -larm_cortexM7lfdp_math

# HAL库的位置
HAL_PATH          := $(CONTRIB_PATH)/HAL
CPPFLAGS          += -I$(HAL_PATH)/Inc

# 系统基础代码的位置
SYS_CORE_PATH     := $(SYS_LOCATION_PATH)/core
CPPFLAGS          += -I$(SYS_CORE_PATH)/include \

# 链接可执行文件的链接脚本
LDSCRIPT          := $(SYS_CORE_PATH)/startup/GCC/LinkerScript.ld
LDFLAGS           += -T $(LDSCRIPT)

# FreeRTOS内核代码
KERNEL_PATH       := $(SYS_LOCATION_PATH)/kernel
FREERTOS_KERNEL   := $(KERNEL_PATH)/FreeRTOS_Kernel

#
OBJS :=

# base system objects
SYSOBJS  = \
    $(OBJDIR)/startup_stm32f767xx.o \
    $(OBJDIR)/stm32f7xx_hal_msp.o   \
    $(OBJDIR)/stm32f7xx_it.o        \
    $(OBJDIR)/syscalls.o            \
    $(OBJDIR)/system_stm32f7xx.o
OBJS += $(SYSOBJS)

HALOBJS  = \
    $(OBJDIR)/stm32f7xx_hal.o              \
    $(OBJDIR)/stm32f7xx_hal_adc.o          \
    $(OBJDIR)/stm32f7xx_hal_adc_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_can.o          \
    $(OBJDIR)/stm32f7xx_hal_cec.o          \
    $(OBJDIR)/stm32f7xx_hal_cortex.o       \
    $(OBJDIR)/stm32f7xx_hal_crc.o          \
    $(OBJDIR)/stm32f7xx_hal_crc_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_cryp.o         \
    $(OBJDIR)/stm32f7xx_hal_cryp_ex.o      \
    $(OBJDIR)/stm32f7xx_hal_dac.o          \
    $(OBJDIR)/stm32f7xx_hal_dac_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_dcmi.o         \
    $(OBJDIR)/stm32f7xx_hal_dcmi_ex.o      \
    $(OBJDIR)/stm32f7xx_hal_dfsdm.o        \
    $(OBJDIR)/stm32f7xx_hal_dma.o          \
    $(OBJDIR)/stm32f7xx_hal_dma_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_dsi.o          \
    $(OBJDIR)/stm32f7xx_hal_eth.o          \
    $(OBJDIR)/stm32f7xx_hal_flash.o        \
    $(OBJDIR)/stm32f7xx_hal_flash_ex.o     \
    $(OBJDIR)/stm32f7xx_hal_gpio.o         \
    $(OBJDIR)/stm32f7xx_hal_hash.o         \
    $(OBJDIR)/stm32f7xx_hal_hash_ex.o      \
    $(OBJDIR)/stm32f7xx_hal_hcd.o          \
    $(OBJDIR)/stm32f7xx_hal_i2s.o          \
    $(OBJDIR)/stm32f7xx_hal_irda.o         \
    $(OBJDIR)/stm32f7xx_hal_iwdg.o         \
    $(OBJDIR)/stm32f7xx_hal_jpeg.o         \
    $(OBJDIR)/stm32f7xx_hal_lptim.o        \
    $(OBJDIR)/stm32f7xx_hal_ltdc.o         \
    $(OBJDIR)/stm32f7xx_hal_ltdc_ex.o      \
    $(OBJDIR)/stm32f7xx_hal_mdios.o        \
    $(OBJDIR)/stm32f7xx_hal_mmc.o          \
    $(OBJDIR)/stm32f7xx_hal_nand.o         \
    $(OBJDIR)/stm32f7xx_hal_nor.o          \
    $(OBJDIR)/stm32f7xx_hal_pcd.o          \
    $(OBJDIR)/stm32f7xx_hal_pcd_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_pwr.o          \
    $(OBJDIR)/stm32f7xx_hal_pwr_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_qspi.o         \
    $(OBJDIR)/stm32f7xx_hal_rcc.o          \
    $(OBJDIR)/stm32f7xx_hal_rcc_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_rng.o          \
    $(OBJDIR)/stm32f7xx_hal_rtc.o          \
    $(OBJDIR)/stm32f7xx_hal_rtc_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_sai.o          \
    $(OBJDIR)/stm32f7xx_hal_sai_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_sd.o           \
    $(OBJDIR)/stm32f7xx_hal_sdram.o        \
    $(OBJDIR)/stm32f7xx_hal_smartcard.o    \
    $(OBJDIR)/stm32f7xx_hal_smartcard_ex.o \
    $(OBJDIR)/stm32f7xx_hal_smbus.o        \
    $(OBJDIR)/stm32f7xx_hal_spdifrx.o      \
    $(OBJDIR)/stm32f7xx_hal_spi.o          \
    $(OBJDIR)/stm32f7xx_hal_sram.o         \
    $(OBJDIR)/stm32f7xx_hal_tim.o          \
    $(OBJDIR)/stm32f7xx_hal_tim_ex.o       \
    $(OBJDIR)/stm32f7xx_hal_uart.o         \
    $(OBJDIR)/stm32f7xx_hal_usart.o        \
    $(OBJDIR)/stm32f7xx_hal_wwdg.o         \
    $(OBJDIR)/stm32f7xx_ll_adc.o           \
    $(OBJDIR)/stm32f7xx_ll_crc.o           \
    $(OBJDIR)/stm32f7xx_ll_dac.o           \
    $(OBJDIR)/stm32f7xx_ll_dma.o           \
    $(OBJDIR)/stm32f7xx_ll_exti.o          \
    $(OBJDIR)/stm32f7xx_ll_fmc.o           \
    $(OBJDIR)/stm32f7xx_ll_gpio.o          \
    $(OBJDIR)/stm32f7xx_ll_lptim.o         \
    $(OBJDIR)/stm32f7xx_ll_pwr.o           \
    $(OBJDIR)/stm32f7xx_ll_rcc.o           \
    $(OBJDIR)/stm32f7xx_ll_rng.o           \
    $(OBJDIR)/stm32f7xx_ll_rtc.o           \
    $(OBJDIR)/stm32f7xx_ll_sdmmc.o         \
    $(OBJDIR)/stm32f7xx_ll_spi.o           \
    $(OBJDIR)/stm32f7xx_ll_tim.o           \
    $(OBJDIR)/stm32f7xx_ll_usart.o         \
    $(OBJDIR)/stm32f7xx_ll_usb.o           \
    $(OBJDIR)/stm32f7xx_ll_utils.o
OBJS += $(HALOBJS)

# FreeRTOS内核代码
FREETROS_KERNEL_COREOBJS =   \
    $(OBJDIR)/croutine.o     \
    $(OBJDIR)/event_groups.o \
    $(OBJDIR)/list.o         \
    $(OBJDIR)/queue.o        \
    $(OBJDIR)/tasks.o        \
    $(OBJDIR)/timers.o
FREETROS_KERNEL_PORTOBJS = \
    $(OBJDIR)/port.o       \
    $(OBJDIR)/heap_4.o
#OBJS += $(FREETROS_KERNEL_COREOBJS) $(FREETROS_KERNEL_PORTOBJS)

include $(CURDIR)/obj.mk

ifeq ($(V), 1)
Q := 
build_slient :=
else
Q := @
build_slient := y
endif

define compile_as
@if [ "$(build_slient)" == "y" ]; then \
$(print) "$(bol)$(color_yellow)AS$(close_color)    $(color_green)$(subst $(ROOTDIR)/,,$<)$(close_color)$(eol)"; \
fi
$(Q)$(AS) $(ASFLAGS) -c $< -o $@
endef

define compile_c
@if [ "$(build_slient)" == "y" ]; then \
$(print) "$(bol)$(color_yellow)CC$(close_color)    $(color_green)$(subst $(ROOTDIR)/,,$<)$(close_color)$(eol)"; \
fi
$(Q)$(CC) $(CPPFLAGS) -c $< -o $@
endef

define link_object
@if [ "$(build_slient)" == "y" ]; then \
$(print) "$(bol)$(color_yellow)LD$(close_color)    $(color_green)$(subst $(ROOTDIR)$(DIR_DIV),,$@)$(close_color)$(eol)"; \
fi
$(Q)$(CC) $(LDFLAGS) $^ -o $@ $(LIBS)
endef

define objcopy_binary
@if [ "$(build_slient)" == "y" ]; then \
$(print) "$(bol)$(color_yellow)GEN$(close_color)   $(color_green)$(subst $(ROOTDIR)$(DIR_DIV),,$@)$(close_color)$(eol)"; \
fi
$(Q)$(OBJCOPY) -I ihex -O binary $< $@
endef

define objcopy_ihex
@if [ "$(build_slient)" == "y" ]; then \
$(print) "$(bol)$(color_yellow)GEN$(close_color)   $(color_green)$(subst $(ROOTDIR)$(DIR_DIV),,$@)$(close_color)$(eol)"; \
fi
$(Q)$(OBJCOPY) -O ihex $< $@
endef

# build options
.PHONY:all clean init
all: init $(BINARY)
	$(Q)$(SIZE) $(ELF)
	@$(print) "$(bol)Success to build the project$(eol)"

print:
	@echo $(OBJS)

clean:
	@echo clean all object files.
	-@cd $(OBJDIR) && $(RM) *.o
	-@$(RM) $(ALT_OBJS)
	-@$(RMDIR) $(OBJDIR)
	@echo clean all output targets.
	-@cd $(O) && $(RM) $(MCU).bin
	-@cd $(O) && $(RM) $(MCU).elf
	-@cd $(O) && $(RM) $(MCU).hex
	@echo remove all directories.
	-@$(RMDIR) $(O)
init:
	@echo Building on the $(OS)
	-@$(MKDIR) $(OBJDIR)

$(ELF): $(OBJS)
	$(call link_object)
$(HEX): $(ELF)
	$(call objcopy_ihex)
$(BINARY): $(HEX)
	$(call objcopy_binary)

# system object files
$(OBJDIR)/startup_stm32f767xx.o: $(SYS_CORE_PATH)/startup/GCC/startup_stm32f767xx.s
	$(call compile_as)
$(OBJDIR)/stm32f7xx_hal_msp.o: $(SYS_CORE_PATH)/stm32f7xx_hal_msp.c
	$(call compile_c)
$(OBJDIR)/stm32f7xx_it.o: $(SYS_CORE_PATH)/stm32f7xx_it.c
	$(call compile_c)
$(OBJDIR)/syscalls.o: $(SYS_CORE_PATH)/syscalls.c
	$(call compile_c)
$(OBJDIR)/system_stm32f7xx.o: $(SYS_CORE_PATH)/system_stm32f7xx.c
	$(call compile_c)

# HAL Drivers Libraries Object Files
$(HALOBJS): $(OBJDIR)/%.o: $(HAL_PATH)/Src/%.c
	$(call compile_c)

# Kernel Objects
$(KERNEL_COREOBJS): $(OBJDIR)/%.o: $( )/%.c
	$(call compile_c)
$(OBJDIR)/port.o: $(FREERTOS_KERNEL)/portable/GCC/ARM_CM7/r0p1/port.c
	$(call compile_c)
$(OBJDIR)/heap_4.o: $(FREERTOS_KERNEL)/portable/MemMang/heap_4.c
	$(call compile_c)

#
$(ALT_OBJS): %.o: %.c
	$(call compile_c)
