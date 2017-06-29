#!/bin/bash
#
# Bareflank Hypervisor
#
# Copyright (C) 2015 Assured Information Security, Inc.
# Author: Bradley Hensel        <henselb@ainfosec.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
#

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

