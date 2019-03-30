                            说明
    本工程只适用于STM32F767IGT6，代码可以随意修改(练手代码, 没有许可证可言)，
使用该工程的代码导致的问题概不负责。

	以下介绍本工程配套使用的一些工具：
1. 编译工具链：arm-none-eabi-gcc (免费, 包含了调试使用的工具arm-none-eabi-gdb)
    https://developer.arm.com/tools-and-software/open-source-software/gnu-toolchain/gnu-rm/downloads
2. 烧录/调试需要用的工具: STLINK-V2
		配合gdb的调试软件: openocd
		烧录代码的软件: st-flash(openocd也可以完成烧录)

	本工程使用的工具在Windows/Linux中都有对应的版本, 通过适当的修改也可以在keil(armcc)中编译调试。
