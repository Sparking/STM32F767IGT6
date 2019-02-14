#! /usr/bin/env bash
script_name=`realpath $0`
current_path=${script_name%/`basename ${script_name}`}
root_path=${current_path%/`basename ${current_path}`}
mkfile=${current_path}/Makefile

if [ -f ${mkdfile} ];then
	make -f ${mkfile} ROOTDIR=${root_path} clean
else
  echo 'Makefile is not found.' 2>&1
fi

