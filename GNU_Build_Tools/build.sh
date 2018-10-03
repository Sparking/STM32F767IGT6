#!/bin/bash

CPUS=`grep -c ^processor /proc/cpuinfo`
script_name=`realpath $0`
script_dir=${script_name%\/`basename ${script_name}`}
prj_root_dir=${script_dir%\/`basename ${script_dir}`}

pushd ${script_dir} > /dev/null

if [ -f ./scripts/update_objects.py ]; then
	python3 ./scripts/update_objects.py
elif ! [ -f ./scripts/objects.mk ]; then
    echo "Can't build this project, cause scripts/objects.mk doesn't exist"
    exit -1
fi

if [ -f Makefile ]; then
    make -j${CPUS} -f ./Makefile all ROOTDIR=${prj_root_dir} RELEASE=debug
else
    echo 'Makefile is not found.' 2>&1
fi

popd > /dev/null

