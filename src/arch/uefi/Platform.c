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
#include <stdint.h>
#include <bfvmcallinterface.h>
#include <bfplatform.h>
#include "Driver.h"

void *platform_alloc_rw(uint64_t len)
{
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS ret;
    status = gBS->AllocatePages(AllocateAnyPages,
                                EfiLoaderData,
                                (len / EFI_PAGE_SIZE) + 1,
                                &ret);
    SASSERT(status);

    return (void *)ret;

Abort:
    return NULL;
}

void *platform_alloc_rwe(uint64_t len)
{
    EFI_STATUS status;
    EFI_PHYSICAL_ADDRESS ret;
    status = gBS->AllocatePages(AllocateAnyPages,
                                EfiLoaderCode,
                                (len / EFI_PAGE_SIZE) + 1,
                                &ret);
    SASSERT(status);

    return (void *)ret;

Abort:
    return NULL;
}

void platform_free_rw(void *addr, uint64_t len)
{
    EFI_STATUS status;
    status = gBS->FreePages((EFI_PHYSICAL_ADDRESS)addr,
                            (len / EFI_PAGE_SIZE) + 1);
    SASSERT(status);

Abort:
    return;
}

void platform_free_rwe(void *addr, uint64_t len)
{
    EFI_STATUS status;
    status = gBS->FreePages((EFI_PHYSICAL_ADDRESS)addr,
                            (len / EFI_PAGE_SIZE) + 1);
    SASSERT(status);

Abort:
    return;
}

void *platform_virt_to_phys(void *virt)
{
    return virt;
}

void *platform_memset(void *ptr, char value, uint64_t num)
{
    gBS->SetMem(ptr, num, value);
    return ptr;
}

void *platform_memcpy(void *dst, const void *src, uint64_t num)
{
    gBS->CopyMem((VOID *)dst, (VOID *)src, num);
    return dst;
}

void platform_start(void)
{}

void platform_stop(void)
{}

int64_t
platform_num_cpus(void)
{
    EFI_STATUS status = EFI_ABORTED;
    UINTN N;
    UINTN NEnabled;

    if (gMpServices != NULL)
        status = gMpServices->GetNumberOfProcessors(gMpServices,
                 &N,
                 &NEnabled);

    if (EFI_ERROR(status)) {
        return 0;
    }

    return N;
}

int64_t
platform_set_affinity(int64_t affinity)
{
    EFI_STATUS status;

    UINTN ret;
    status = gMpServices->WhoAmI(gMpServices,
                                 &ret);
    if (status != EFI_SUCCESS) {
        Print(L"WhoAmI error %r\n", status);
        return -1;
    }

    if (ret == affinity) {
        return ret;
    }

    status = gMpServices->EnableDisableAP(gMpServices,
                                          affinity,
                                          TRUE,
                                          NULL);
    if (status != EFI_SUCCESS) {
        Print(L"EnableDisableAP error %r\n", status);
        return -1;
    }

    status = gMpServices->SwitchBSP(gMpServices,
                                    affinity,
                                    TRUE);
    if (status != EFI_SUCCESS) {
        Print(L"SwitchBSP error %r\n", status);
        return -1;
    }

    return ret;
}

void
platform_restore_affinity(int64_t affinity)
{
    Print(L"platform_restore_affinity %d\n", affinity);
    platform_set_affinity(affinity);
}

int64_t platform_get_current_cpu_num(void)
{
    EFI_STATUS status;

    UINTN ret;
    status = gMpServices->WhoAmI(gMpServices,
                                 &ret);
    if (EFI_ERROR(status)) { goto abort; }

    return ret;

abort:
    Print(L"platform_get_current_cpu_num error %r\n", status);
    return -1;
}

void platform_restore_preemption(void)
{}
