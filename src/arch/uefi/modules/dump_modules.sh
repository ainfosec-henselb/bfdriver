#!/bin/bash

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
