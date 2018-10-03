#user's object files:
USEROBJS =  \
    $(OBJDIR)/Src_user_pid.o \
    $(OBJDIR)/Src_user_gui.o \
    $(OBJDIR)/Src_main.o \
    $(OBJDIR)/Src_user_lcd_touch.o \
    $(OBJDIR)/Src_user_setting_interface.o \
    $(OBJDIR)/Src_drive_lcd_touch.o \
    $(OBJDIR)/Src_drive_at24cxx.o \
    $(OBJDIR)/Src_drive_oled.o \
    $(OBJDIR)/Src_drive_lcd.o \
    $(OBJDIR)/Src_drive_usart.o \
    $(OBJDIR)/Src_drive_spi.o \
    $(OBJDIR)/Src_drive_numberlib.o \
    $(OBJDIR)/Src_drive_iwdg.o \
    $(OBJDIR)/Src_drive_delay.o \
    $(OBJDIR)/Src_drive_timer.o \
    $(OBJDIR)/Src_drive_softspi.o \
    $(OBJDIR)/Src_drive_digital_filter.o \
    $(OBJDIR)/Src_drive_gt9147.o \
    $(OBJDIR)/Src_drive_ad7705_softspi.o \
    $(OBJDIR)/Src_drive_sdram.o \
    $(OBJDIR)/Src_drive_gpio.o \
    $(OBJDIR)/Src_drive_iic.o \
    $(OBJDIR)/Src_drive_ds18b20.o \
    $(OBJDIR)/Src_drive_exti.o \
    $(OBJDIR)/Src_drive_rtc.o \
    $(OBJDIR)/Src_drive_font.o
#user's object compile:
$(OBJDIR)/Src_user_pid.o: $(ROOTDIR)/Src/user_pid.c
	$(call compile_c)
$(OBJDIR)/Src_user_gui.o: $(ROOTDIR)/Src/user_gui.c
	$(call compile_c)
$(OBJDIR)/Src_main.o: $(ROOTDIR)/Src/main.c
	$(call compile_c)
$(OBJDIR)/Src_user_lcd_touch.o: $(ROOTDIR)/Src/user_lcd_touch.c
	$(call compile_c)
$(OBJDIR)/Src_user_setting_interface.o: $(ROOTDIR)/Src/user_setting_interface.c
	$(call compile_c)
$(OBJDIR)/Src_drive_lcd_touch.o: $(ROOTDIR)/Src/drive/lcd_touch.c
	$(call compile_c)
$(OBJDIR)/Src_drive_at24cxx.o: $(ROOTDIR)/Src/drive/at24cxx.c
	$(call compile_c)
$(OBJDIR)/Src_drive_oled.o: $(ROOTDIR)/Src/drive/oled.c
	$(call compile_c)
$(OBJDIR)/Src_drive_lcd.o: $(ROOTDIR)/Src/drive/lcd.c
	$(call compile_c)
$(OBJDIR)/Src_drive_usart.o: $(ROOTDIR)/Src/drive/usart.c
	$(call compile_c)
$(OBJDIR)/Src_drive_spi.o: $(ROOTDIR)/Src/drive/spi.c
	$(call compile_c)
$(OBJDIR)/Src_drive_numberlib.o: $(ROOTDIR)/Src/drive/numberlib.c
	$(call compile_c)
$(OBJDIR)/Src_drive_iwdg.o: $(ROOTDIR)/Src/drive/iwdg.c
	$(call compile_c)
$(OBJDIR)/Src_drive_delay.o: $(ROOTDIR)/Src/drive/delay.c
	$(call compile_c)
$(OBJDIR)/Src_drive_timer.o: $(ROOTDIR)/Src/drive/timer.c
	$(call compile_c)
$(OBJDIR)/Src_drive_softspi.o: $(ROOTDIR)/Src/drive/softspi.c
	$(call compile_c)
$(OBJDIR)/Src_drive_digital_filter.o: $(ROOTDIR)/Src/drive/digital_filter.c
	$(call compile_c)
$(OBJDIR)/Src_drive_gt9147.o: $(ROOTDIR)/Src/drive/gt9147.c
	$(call compile_c)
$(OBJDIR)/Src_drive_ad7705_softspi.o: $(ROOTDIR)/Src/drive/ad7705_softspi.c
	$(call compile_c)
$(OBJDIR)/Src_drive_sdram.o: $(ROOTDIR)/Src/drive/sdram.c
	$(call compile_c)
$(OBJDIR)/Src_drive_gpio.o: $(ROOTDIR)/Src/drive/gpio.c
	$(call compile_c)
$(OBJDIR)/Src_drive_iic.o: $(ROOTDIR)/Src/drive/iic.c
	$(call compile_c)
$(OBJDIR)/Src_drive_ds18b20.o: $(ROOTDIR)/Src/drive/ds18b20.c
	$(call compile_c)
$(OBJDIR)/Src_drive_exti.o: $(ROOTDIR)/Src/drive/exti.c
	$(call compile_c)
$(OBJDIR)/Src_drive_rtc.o: $(ROOTDIR)/Src/drive/rtc.c
	$(call compile_c)
$(OBJDIR)/Src_drive_font.o: $(ROOTDIR)/Src/drive/font.c
	$(call compile_c)
