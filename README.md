                               使用说明
    本工程只适用于STM32F767IGT6，代码可以随意修改(练手代码, 没有许可证可言)，使用该工程的代码导致的问题本人概不负责。

	工程配套使用的一些工具：
    1. 编译工具链：arm-none-eabi-gcc (免费, 包含了调试使用的工具arm-none-eabi-gdb)
        https://developer.arm.com/tools-and-software/open-source-software/gnu-toolchain/gnu-rm/downloads
    2. 烧录/调试需要用的工具: STLINK-V2
		配合gdb的调试软件: openocd
		烧录代码的软件: st-flash(openocd也可以完成烧录)

	工程使用的工具在Windows/Linux中都有对应的版本(Windows中的版本比较难找), 通过适当的修改也可以在keil(armcc)中编译调试。


构建代码说明:
  1.Makefile和obj.mk
	工程根目录下有个两个文件，分别是Makefile和obj.mk。
	Makefile: 整个工程的编译入口都在这里
	obj.mk: 由于工程下的c文件比较多，一般固定不变化的基础代码直接放到Makefile中编译。
	    为了避免和基础代码的编译造成混乱，所有由用户自己实现的代码都单独的由该文件来控制是否编译。
  2.构建代码的命令:
	#make          ==> 开始编译
	#make V=1      ==> 显示完整的编译过程
	#make RELEASE=release ==> 编译成release版本，这个版本的二进制文件比较小，但不方便调试
	#make clean    ==> 清除编译的内容
	#make st-flash ==> 使用st-flash命令烧录代码
  3.编译的输出
	编译过程期间，make将在代码根目录建立Build文件夹，编译的最终输出都在该目录下
	如果编译成功，Build目录下将会生成3个文件：
		stm32f767igt6.bin # 二进制文件，实际上flash中的内容就是这个文件，用于st-flash烧录
		stm32f767igt6.elf # 链接产生的elf文件，该文件提供给gdb调试
		stm32f767igt6.hex # Intel格式的hex文件，openocd烧录需要用到
    除了以上3个文件之外，还有一个额外的目录GCC_Objects，该目录用于存放非用户代码的.o文件，而用户自己实现的代码
	生成的.o文件和.c文件放在同一个目录。
  4.代码的存放目录说明:
    driver: 驱动
	   lib: 公用的库，与平台无关
	   src: 用户主程序的代码
	   sys: 系统的基础代码，该目录的内容一般是固定的，最好不要把自己实现的代码放到这里，不然会混淆
	        sys下的代码编译后的.o文件统一放在目录GCC_Objects下，不过这不是强制的(历史原因，现在不想改这一块了)

使用st-link烧录(make st-flash)
st-flash是st-link组件中的一个命令，Ubuntu软件源没有包含这个组件，需要自行从官方下载源码编译(Windows有对应的二进制文件，省去了编译)
1. 源码:
https://github.com/Sparking/stlink(被我修改了一些代码，让输出更加漂亮一些)
https://github.com/texane/stlink (官方源码,官网也介绍了组件的使用情况)
2. 将代码下载好之后，开始编译:
#apt install libusb-1.0-0-dev cmake ==> 需要预先安装libusb库和cmake命令
#cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/ . && make && make install ==> 进入代码根目录运行该命令
#echo "/usr/local/lib/" >> /etc/ld.so.conf.d/x86_64-linux-gnu.conf && ldconfig -f /etc/ld.so.conf.d/x86_64-linux-gnu.conf
    ==> 添加动态库的搜索路径
3. 编译结束后可以运行命令st-flash --help确认安装是否成功


使用openocd烧录代码
1.执行以下命令，将openocd通过STLINK-v2连接到开发板:
  #openocd -f target/stm32f7x.cfg -t interface/stlink-v2.cfg ==> 执行成功后，openocd能够打印出芯片的详细信息
2.通过telnet向openocd发送命令，首先要telnet登录到openocd的控制台中:
  #telnet localhost 4444 ==> 登录到openocd的控制台，可以输入命令
3.进入控制台后，敲入以下的命令:
  #reset halt ==> 先停止设备的运行
  #flash write_image erase ./Build/stm32f767igt6.hex ==> 将代码烧录至芯片的flash中
  #reset ==> 重置芯片的状态，并开始运行
  #exit  ==> 烧入结束后即可退出，并断开openocd和开发板的链接
以上内容可以通过脚本实现一键烧录，但目前尚未去实现


使用arm-none-eabi-gdb和openocd进行调试
有的时候需要上设备调试代码，这时可以使用gdb，具体方法如下:
1.和使用openocd烧录代码一样，先建立openocd和开发板的链接:
  #openocd -f target/stm32f7x.cfg -t interface/stlink-v2.cfg
2.使用gdb开始调试；
  #arm-none-eabi-gdb Build/stm32f767igt6.elf
  ==>接下来进入gdb中
  (gdb) target remote:3333 ==> 链接成功后，gdb会打印当前执行的函数，一般是??，不用理会
  (gdb) load               ==> 将elf文件中的一些段加载至设备flash中
  (gdb) ......             ==> 这里和在Linux下调试程序一样，可是设置断点，看堆栈登
  (gdb) continue           ==> 开始执行代码(不是run命令)到断点位置
  .......
