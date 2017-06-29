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
rm -f modules.list
rm -f ./*.c

if [ -z "$BF_LIBDIR" ]; then
    echo "Variable BF_LIBDIR needs to be set to the location of hypervisor shared objects";
    exit 1;
fi

files="libpthread.so
libbfunwind.so
libc++.so.1
libc++abi.so.1
libc.so
libbfsyscall.so
libbfvmm_vcpu_manager.so
libbfvmm_serial.so
libbfvmm_support.so
libbfvmm_vcpu_factory.so
libbfvmm_vcpu.so
libbfvmm_debug_ring.so
libbfvmm_exit_handler.so
libbfvmm_vmcs.so
libbfvmm_vmxon.so
libbfvmm_memory_manager.so
libbfvmm_intrinsics.so"

for file in ${files}
do
  cp ${BF_LIBDIR}/${file} .
  xxd -i $file > ${file}.c
done


libs=`echo -n ${files} | sed 's/\./_/g' | sed 's/+/_/g'`

for lib in ${libs}
do
  echo ${lib} | sed 's/\(^.\+$\)/HYPERVISOR_MODULE\(\1\)/' >> modules.list
done

VMMNAME=bfvmm
cp ${BF_LIBDIR}/../bin/$VMMNAME .
xxd -i $VMMNAME > $VMMNAME.c
echo "HYPERVISOR_MODULE($VMMNAME)" >> modules.list
