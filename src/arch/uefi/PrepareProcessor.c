/*
 * Bareflank Hypervisor
 *
 * Copyright (C) 2015 Assured Information Security, Inc.
 * Author: Bradley Hensel        <henselb@ainfosec.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include "x86_64.h"

EFI_STATUS
PrepareProcessor()
{
    _writeeflags(_readeflags() & ~(1 << 18));

    PKGDTENTRY64 TssEntry, NewGdt;
    PKTSS64 Tss;
    KDESCRIPTOR Gdtr;

    _sgdt((void *)&Gdtr.Limit);

    NewGdt = (PKGDTENTRY64)AllocateRuntimeZeroPool(MAX(Gdtr.Limit + 1,
             KGDT64_SYS_TSS + sizeof(*TssEntry)));
    if (NewGdt == NULL) {
        return EFI_OUT_OF_RESOURCES;
    }

    gBS->CopyMem(NewGdt, Gdtr.Base, Gdtr.Limit + 1);

    Tss = AllocateRuntimeZeroPool(sizeof(*Tss) * 2);
    if (Tss == NULL) {
        gBS->FreePool(NewGdt);
        return EFI_OUT_OF_RESOURCES;
    }

    TssEntry = (PKGDTENTRY64)((uintptr_t)NewGdt + KGDT64_SYS_TSS);
    TssEntry->BaseLow = (uintptr_t)Tss & 0xffff;
    TssEntry->Bits.BaseMiddle = ((uintptr_t)Tss >> 16) & 0xff;
    TssEntry->Bits.BaseHigh = ((uintptr_t)Tss >> 24) & 0xff;
    TssEntry->BaseUpper = (uintptr_t)Tss >> 32;
    TssEntry->LimitLow = sizeof(KTSS64) - 1;
    TssEntry->Bits.Type = AMD64_TSS;
    TssEntry->Bits.Dpl = 0;
    TssEntry->Bits.Present = 1;
    TssEntry->Bits.System = 0;
    TssEntry->Bits.LongMode = 0;
    TssEntry->Bits.DefaultBig = 0;
    TssEntry->Bits.Granularity = 0;
    TssEntry->MustBeZero = 0;

    Gdtr.Base = NewGdt;
    Gdtr.Limit = KGDT64_SYS_TSS + sizeof(*TssEntry);
    _lgdt((void *)&Gdtr.Limit);

    _ltr(KGDT64_SYS_TSS);

    _unset_vmxe();

    return EFI_SUCCESS;
}

