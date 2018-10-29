#user's object files:
USEROBJS =  \
    $(OBJDIR)/Src_user_lcd_touch.o \
    $(OBJDIR)/Src_main.o \
    $(OBJDIR)/Src_user_pid.o \
    $(OBJDIR)/Src_user_gui.o \
    $(OBJDIR)/Src_user_setting_interface.o \
    $(OBJDIR)/Src_driver_iic.o \
    $(OBJDIR)/Src_driver_ds18b20.o \
    $(OBJDIR)/Src_driver_timer.o \
    $(OBJDIR)/Src_driver_gt9147.o \
    $(OBJDIR)/Src_driver_ad7705_softspi.o \
    $(OBJDIR)/Src_driver_softspi.o \
    $(OBJDIR)/Src_driver_oled.o \
    $(OBJDIR)/Src_driver_at24cxx.o \
    $(OBJDIR)/Src_driver_font.o \
    $(OBJDIR)/Src_driver_exti.o \
    $(OBJDIR)/Src_driver_numberlib.o \
    $(OBJDIR)/Src_driver_sdram.o \
    $(OBJDIR)/Src_driver_lcd.o \
    $(OBJDIR)/Src_driver_iwdg.o \
    $(OBJDIR)/Src_driver_spi.o \
    $(OBJDIR)/Src_driver_delay.o \
    $(OBJDIR)/Src_driver_digital_filter.o \
    $(OBJDIR)/Src_driver_mpu9250.o \
    $(OBJDIR)/Src_driver_usart.o \
    $(OBJDIR)/Src_driver_rtc.o \
    $(OBJDIR)/Src_driver_gpio.o \
    $(OBJDIR)/Src_driver_lcd_touch.o
#user's object compile:
$(OBJDIR)/Src_user_lcd_touch.o: $(ROOTDIR)/Src/user_lcd_touch.c
	$(call compile_c)
$(OBJDIR)/Src_main.o: $(ROOTDIR)/Src/main.c
	$(call compile_c)
$(OBJDIR)/Src_user_pid.o: $(ROOTDIR)/Src/user_pid.c
	$(call compile_c)
$(OBJDIR)/Src_user_gui.o: $(ROOTDIR)/Src/user_gui.c
	$(call compile_c)
$(OBJDIR)/Src_user_setting_interface.o: $(ROOTDIR)/Src/user_setting_interface.c
	$(call compile_c)
$(OBJDIR)/Src_driver_iic.o: $(ROOTDIR)/Src/driver/iic.c
	$(call compile_c)
$(OBJDIR)/Src_driver_ds18b20.o: $(ROOTDIR)/Src/driver/ds18b20.c
	$(call compile_c)
$(OBJDIR)/Src_driver_timer.o: $(ROOTDIR)/Src/driver/timer.c
	$(call compile_c)
$(OBJDIR)/Src_driver_gt9147.o: $(ROOTDIR)/Src/driver/gt9147.c
	$(call compile_c)
$(OBJDIR)/Src_driver_ad7705_softspi.o: $(ROOTDIR)/Src/driver/ad7705_softspi.c
	$(call compile_c)
$(OBJDIR)/Src_driver_softspi.o: $(ROOTDIR)/Src/driver/softspi.c
	$(call compile_c)
$(OBJDIR)/Src_driver_oled.o: $(ROOTDIR)/Src/driver/oled.c
	$(call compile_c)
$(OBJDIR)/Src_driver_at24cxx.o: $(ROOTDIR)/Src/driver/at24cxx.c
	$(call compile_c)
$(OBJDIR)/Src_driver_font.o: $(ROOTDIR)/Src/driver/font.c
	$(call compile_c)
$(OBJDIR)/Src_driver_exti.o: $(ROOTDIR)/Src/driver/exti.c
	$(call compile_c)
$(OBJDIR)/Src_driver_numberlib.o: $(ROOTDIR)/Src/driver/numberlib.c
	$(call compile_c)
$(OBJDIR)/Src_driver_sdram.o: $(ROOTDIR)/Src/driver/sdram.c
	$(call compile_c)
$(OBJDIR)/Src_driver_lcd.o: $(ROOTDIR)/Src/driver/lcd.c
	$(call compile_c)
$(OBJDIR)/Src_driver_iwdg.o: $(ROOTDIR)/Src/driver/iwdg.c
	$(call compile_c)
$(OBJDIR)/Src_driver_spi.o: $(ROOTDIR)/Src/driver/spi.c
	$(call compile_c)
$(OBJDIR)/Src_driver_delay.o: $(ROOTDIR)/Src/driver/delay.c
	$(call compile_c)
$(OBJDIR)/Src_driver_digital_filter.o: $(ROOTDIR)/Src/driver/digital_filter.c
	$(call compile_c)
$(OBJDIR)/Src_driver_mpu9250.o: $(ROOTDIR)/Src/driver/mpu9250.c
	$(call compile_c)
$(OBJDIR)/Src_driver_usart.o: $(ROOTDIR)/Src/driver/usart.c
	$(call compile_c)
$(OBJDIR)/Src_driver_rtc.o: $(ROOTDIR)/Src/driver/rtc.c
	$(call compile_c)
$(OBJDIR)/Src_driver_gpio.o: $(ROOTDIR)/Src/driver/gpio.c
	$(call compile_c)
$(OBJDIR)/Src_driver_lcd_touch.o: $(ROOTDIR)/Src/driver/lcd_touch.c
	$(call compile_c)
