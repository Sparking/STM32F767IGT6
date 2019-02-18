#Directory
ROOTDIR    = ..

ifeq ($(OS),Windows_NT)
DIR_DIV  = \\
MKDIR    = mkdir
RMDIR    = rmdir
RM       = del -f
print    = echo
else
OS       = $(shell uname)
DIR_DIV  = /
MKDIR    = mkdir -p
RMDIR    = rmdir
RM       = rm -f
print    = printf
color_red    = \\033[31m
color_green  = \\033[32m
color_yellow = \\033[33m
close_color  = \\033[0m
bol          = \\r\\033[K
eol          = \\r\\n
endif

OUTPUTDIR  = $(ROOTDIR)$(DIR_DIV)Build
OBJDIR     = $(OUTPUTDIR)$(DIR_DIV)GCC_Objects
COMMON_DIR = $(ROOTDIR)/common
KERNEL_DIR = $(ROOTDIR)/System/FreeRTOS_Kernel

#Images
LDSCRIPT = $(ROOTDIR)/System/lib/LinkerScript.ld
MCU      = stm32f767igt6
BINARY   = $(OUTPUTDIR)/$(MCU).bin
ELF      = $(OUTPUTDIR)/$(MCU).elf
HEX      = $(OUTPUTDIR)/$(MCU).hex

#cpu settings
CORE    = cortex-m7
FPUABI  = hard
FPUTYPE = fpv5-d16

#compiler and other tools
PLATFORM = arm-none-eabi
CC       = $(PLATFORM)-gcc
AS       = $(PLATFORM)-as
LD       = $(PLATFORM)-ld
SIZE     = $(PLATFORM)-size
OBJCOPY  = $(PLATFORM)-objcopy

#complier's flags
RELEASE = debug
ifeq ($(RELEASE), release)
CODE_OPT_FLAGS = -Os
DEBUG_FLAGS =
else ifeq ($(RELEASE), debug)
CODE_OPT_FLAGS = -O0
DEBUG_FLAGS = -gdwarf-2
endif

FILE_OPT_FLAGS = -ffunction-sections -fdata-sections
CSYNTAXSTD     = -std=gnu99
TARGET_FLAGS   = -mthumb -mcpu=$(CORE)
FLOAT_FLAGS    = -mfloat-abi=$(FPUABI) -mfpu=$(FPUTYPE) -u _printf_float
ASFLAGS        = $(TARGET_FLAGS) $(DEBUG_FLAG) $(FLOAT_FLAG)
SPECS_OPT      = --specs=nano.specs --specs=nosys.specs
WARNING_RANK   = -Wall -Werror
MARCO_DEF      = \
    -DUSE_HAL_DRIVER \
    -DSTM32F767xx    \
    -DARM_MATH_CM7   \
    -D__FPU_PRESENT  \
    -D__FPU_USED=1U  \
    -DUNALIGNED_SUPPORT_DISABLE
INC_OPT = \
	-I$(COMMON_DIR)/include            \
    -I$(ROOTDIR)/Src/include           \
    -I$(ROOTDIR)/Src/driver/include    \
    -I$(ROOTDIR)/System/core           \
    -I$(ROOTDIR)/System/device/include \
    -I$(ROOTDIR)/System/HAL_Driver/Inc \
    -I$(KERNEL_DIR)/                   \
    -I$(KERNEL_DIR)/include            \
    -I$(KERNEL_DIR)/portable/GCC/ARM_CM7/r0p1
CPPFLAGS = \
    $(CSYNTAXSTD) $(DEBUG_FLAGS) $(WARNING_RANK) \
    $(TARGET_FLAGS) $(FLOAT_FLAGS) $(SPECS_OPT)  \
    $(MARCO_DEF) $(INC_OPT) $(FILE_OPT_FLAGS)
LDFLAGS = \
    $(CODE_OPT_FLAGS) \
    $(FILE_OPT_FLAGS) \
    -Wl,-gc-section   \
    $(DEBUG_OPT)      \
    $(TARGET_FLAGS)   \
    $(FLOAT_FLAGS)    \
    $(SPECS_OPT)
LIBRARY_OPT = \
    -L$(ROOTDIR)/System/lib \
    -larm_cortexM7lfdp_math \
    -lm

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
$(Q)$(CC) $(CODE_OPT_FLAGS) $(CPPFLAGS) -c $< -o $@
endef

define link_object
@if [ "$(build_slient)" == "y" ]; then \
$(print) "$(bol)$(color_yellow)LD$(close_color)    $(color_green)$(subst $(ROOTDIR)$(DIR_DIV),,$@)$(close_color)$(eol)"; \
fi
$(Q)$(CC) $(LDFLAGS) -T $(LDSCRIPT) $^ -o $@ $(LIBRARY_OPT)
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

# common objects
COMMON_OBJS = \
	$(OBJDIR)/bufchain.o \
	$(OBJDIR)/readline.o \
	$(OBJDIR)/cli.o

# base system objects
SYSOBJS  = \
    $(OBJDIR)/startup_stm32f767xx.o \
    $(OBJDIR)/stm32f7xx_hal_msp.o   \
    $(OBJDIR)/stm32f7xx_it.o        \
    $(OBJDIR)/syscalls.o            \
    $(OBJDIR)/system_stm32f7xx.o

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
    $(OBJDIR)/stm32f7xx_hal_dma2d.o        \
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
    $(OBJDIR)/stm32f7xx_ll_dma2d.o         \
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

# kernel object files
KERNEL_COREOBJS = \
    $(OBJDIR)/croutine.o    \
    $(OBJDIR)/event_groups.o\
    $(OBJDIR)/list.o        \
    $(OBJDIR)/queue.o       \
    $(OBJDIR)/tasks.o       \
    $(OBJDIR)/timers.o
KERNEL_PORTOBJS = \
    $(OBJDIR)/port.o        \
    $(OBJDIR)/heap_4.o

#build options
.PHONY:all clean init
all: init $(BINARY)
	$(Q)$(SIZE) $(ELF)
	@$(print) "$(bol)Success to build the project$(eol)"

include scripts/objects.mk

OBJS = $(HALOBJS) $(SYSOBJS) $(KERNEL_COREOBJS) $(KERNEL_PORTOBJS) $(USEROBJS) $(COMMON_OBJS)

clean:
	@echo clean all object files.
	-@cd $(OBJDIR) && $(RM) *.o
	-@$(RMDIR) $(OBJDIR)
	@echo clean all output targets.
	-@cd $(OUTPUTDIR) && $(RM) $(MCU).bin
	-@cd $(OUTPUTDIR) && $(RM) $(MCU).elf
	-@cd $(OUTPUTDIR) && $(RM) $(MCU).hex
	@echo remove all directories.
	-@${RMDIR} $(OUTPUTDIR)
init:
	@echo Building on the $(OS)
	-@$(MKDIR) $(OBJDIR)
$(ELF): $(OBJS)
	$(call link_object)
$(HEX): $(ELF)
	$(call objcopy_ihex)
$(BINARY): $(HEX)
	$(call objcopy_binary)

#system object files
$(OBJDIR)/startup_stm32f767xx.o: $(ROOTDIR)/System/device/startup/GCC/startup_stm32f767xx.s
	$(call compile_as)
$(OBJDIR)/stm32f7xx_hal_msp.o: $(ROOTDIR)/System/device/stm32f7xx_hal_msp.c
	$(call compile_c)
$(OBJDIR)/stm32f7xx_it.o: $(ROOTDIR)/System/device/stm32f7xx_it.c
	$(call compile_c)
$(OBJDIR)/syscalls.o: $(ROOTDIR)/System/device/syscalls.c
	$(call compile_c)
$(OBJDIR)/system_stm32f7xx.o: $(ROOTDIR)/System/device/system_stm32f7xx.c
	$(call compile_c)

#HAL Drivers Libraries Object Files
HAL_DRIVER_DIR = $(ROOTDIR)/System/HAL_Driver
$(HALOBJS): $(OBJDIR)/%.o: $(HAL_DRIVER_DIR)/Src/%.c
	$(call compile_c)

#Kernel Objects
$(KERNEL_COREOBJS): $(OBJDIR)/%.o: $(KERNEL_DIR)/%.c
	$(call compile_c)
$(OBJDIR)/port.o: $(KERNEL_DIR)/portable/GCC/ARM_CM7/r0p1/port.c
	$(call compile_c)
$(OBJDIR)/heap_4.o: $(KERNEL_DIR)/portable/MemMang/heap_4.c
	$(call compile_c)

#common objects
$(COMMON_OBJS): $(OBJDIR)/%.o: $(COMMON_DIR)/%.c
	$(call compile_c)

