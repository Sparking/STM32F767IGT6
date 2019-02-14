#! /usr/bin/env bash
script_name=`realpath $0`
current_path=${script_name%/`basename ${script_name}`}
root_path=${current_path%/`basename ${current_path}`}
img=${root_path}/Build/stm32f767igt6.bin

if [ -f ${img} ];then
  echo "loading `basename ${img}` to device..."
  st-flash erase && st-flash write ${img} 0x08000000
else
  echo ${img} is not found! 2>&1
fi
