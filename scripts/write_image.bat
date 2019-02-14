@echo off
echo Start to load Image to STM32F767IGT6
if exist ..\Build\STM32F767IGT6.bin st-flash erase
if exist ..\Build\STM32F767IGT6.bin st-flash write ..\Build\STM32F767IGT6.bin 0x08000000
