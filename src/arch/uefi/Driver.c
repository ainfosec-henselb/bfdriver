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

#include "Driver.h"
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/BaseLib.h>
#include <Protocol/DevicePath.h>
#include <Protocol/LoadedImage.h>
#include "bfelf_loader.h"
#include "bferrorcodes.h"
#include "Modules.h"
#include "common.h"
#include "x86_64.h"

EFI_HANDLE gImage;
EFI_MP_SERVICES_PROTOCOL *gMpServices = NULL;

EFI_STATUS
EFIAPI
UefiMain (
    IN EFI_HANDLE        ImageHandle,
    IN EFI_SYSTEM_TABLE  *SystemTable
)
{
    Print(L"\nHypervisor Tester\n");
    common_init();

    EFI_STATUS status;
    gImage = ImageHandle;

    status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid,
                                 NULL,
                                 (VOID **)&gMpServices);
    SASSERT(status);

    UINTN cpus;
    UINTN ecpus;
    status = gMpServices->GetNumberOfProcessors(gMpServices,
             &cpus,
             &ecpus);
    SASSERT(status);

    add_hypervisor_modules();

    int64_t ret = common_load_vmm();
    if (ret < 0) {
        Print(L"common_load_vmm returned %a\n", ec_to_str(ret));
        goto Abort;
    }

    int i;
    for (i = 0; i < cpus; i++) {
        Print(L"Starting VMM on core %d\n", i);

        if (i + 1 < cpus) {
            platform_set_affinity(i + 1);
            _set_vmxe();
        }

        platform_set_affinity(i);

        PrepareProcessor();

        ret = common_start_core(i);
        if (ret < 0) {
            goto Abort;
        }
    }

    Print(L"VMM started.\n");

    Print(L"Loading OS\n");
    EFI_DEVICE_PATH_PROTOCOL *TargetDP;
    status = GenerateFileDP(L"\\EFI\\Boot\\native.efi",
                            &TargetDP);

    SASSERT(status);

    EFI_HANDLE TargetHandle;
    status = gBS->LoadImage(TRUE,
                            ImageHandle,
                            TargetDP,
                            NULL,
                            0,
                            &TargetHandle);
    SASSERT(status);

    UINTN ExitDataSize;
    CHAR16 *ExitData;
    status = gBS->StartImage(TargetHandle,
                             &ExitDataSize,
                             &ExitData);
    SASSERT(status);

Abort:
    Print(L"Failed to start hypervisor.\n");
    GetKeystroke(NULL);
    return status;
}
