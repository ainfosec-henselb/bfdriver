#!/bin/bash

set -e

if [ -z "$EDK_PATH" ]; then
    echo "Variable EDK_PATH needs to be set to the absolute location of your EDK tree";
    exit 1;
fi

if [ ! -d "$EDK_PATH" ]; then
    echo "Directory pointed to by EDK_PATH variable doesn't exist"
    exit 1;
fi

PWD=`pwd`
PKGNAME="HypervisorPkg"
PKG="$EDK_PATH/$PKGNAME"

export BF_LIBDIR=`cd "../../../../bfprefix/sysroots/x86_64-vmm-elf/lib" && pwd`

mkdir -p $PKG
cp -R ./* $PKG
( cd ../../../.. && ln -fs $(pwd) $PKG/hypervisor )
( cd $PKG/modules && ./dump_modules.sh )
( cd $EDK_PATH && source ./edksetup.sh && build -t GCC49 -a X64 -b DEBUG -p $PKGNAME/$PKGNAME.dsc )

