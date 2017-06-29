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
#include <Protocol/LoadedImage.h>
#include <Protocol/DevicePathUtilities.h>
#include <Protocol/SimpleFileSystem.h>
#include <Guid/FileInfo.h>
#include "Driver.h"

EFI_STATUS
PrintDevicePath(
    IN EFI_DEVICE_PATH_PROTOCOL *DPIn
)
{
    EFI_DEVICE_PATH_PROTOCOL *it = DPIn;
    CHAR8 *traveler = (CHAR8 *)DPIn;

    if (it == NULL) {
        return EFI_NOT_FOUND;
    }

    while (it->Type != 0x7F) {
        Print (L"Node type: %x  subtype: %x\n", it->Type, it->SubType);

        if (it->Type == 0x04 && it->SubType == 0x04) {
            Print((CHAR16 *)(traveler + 4));
        }

        traveler += *(UINT16 *)it->Length;
        it = (EFI_DEVICE_PATH_PROTOCOL *)traveler;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
GenerateFileDP(
    IN CHAR16 *TargetPath,
    OUT EFI_DEVICE_PATH_PROTOCOL **FileDP
)
{

    if (FileDP == NULL || TargetPath == NULL ||
            StrLen(TargetPath) == 0) {
        return EFI_INVALID_PARAMETER;
    }

    EFI_STATUS status;
    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;

    status = gBS->HandleProtocol(gImage,
                                 &gEfiLoadedImageProtocolGuid,
                                 (VOID **)&LoadedImage);

    SASSERT(status);

    EFI_DEVICE_PATH_PROTOCOL *TargetDevice;
    status = gBS->HandleProtocol(LoadedImage->DeviceHandle,
                                 &gEfiDevicePathProtocolGuid,
                                 (VOID **)&TargetDevice);

    SASSERT(status);

    UINTN TargetPathNodeSize = StrSize(TargetPath) + 4;
    EFI_DEVICE_PATH_PROTOCOL *TargetPathNode;
    status = gBS->AllocatePool(EfiLoaderData,
                               TargetPathNodeSize,
                               (VOID **)&TargetPathNode);

    SASSERT(status);

    TargetPathNode->Type = 0x04;
    TargetPathNode->SubType = 0x04;
    *(UINT16 *)TargetPathNode->Length = TargetPathNodeSize;

    CHAR8 *inserter = (CHAR8 *)TargetPathNode;
    inserter += 4;

    StrCpy((CHAR16 *)inserter, TargetPath);

    EFI_DEVICE_PATH_UTILITIES_PROTOCOL *DPU;
    gBS->LocateProtocol(&gEfiDevicePathUtilitiesProtocolGuid,
                        NULL,
                        (VOID **)&DPU);

    SASSERT(status);

    *FileDP = DPU->AppendDeviceNode(TargetDevice, TargetPathNode);
    gBS->FreePool((VOID *)TargetPathNode);

    return status;

Abort:
    return status;
}

EFI_STATUS
ReadFile(
    IN CHAR16 *TargetPath,
    OUT VOID **Buffer,
    OUT UINTN *BufferSize
)
{
    EFI_STATUS status;

    if (Buffer == NULL || BufferSize == NULL ||
            TargetPath == NULL || StrLen(TargetPath) == 0) {
        return EFI_INVALID_PARAMETER;
    }

    EFI_LOADED_IMAGE_PROTOCOL *LoadedImage;
    status = gBS->HandleProtocol(gImage,
                                 &gEfiLoadedImageProtocolGuid,
                                 (VOID **)&LoadedImage);
    SASSERT(status);

    EFI_HANDLE CurrentDevice = LoadedImage->DeviceHandle;
    if (CurrentDevice == NULL) {
        status = EFI_NOT_FOUND;
        SASSERT(status);
    }

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL *FS;
    status = gBS->HandleProtocol(CurrentDevice,
                                 &gEfiSimpleFileSystemProtocolGuid,
                                 (VOID **)&FS);
    SASSERT(status);

    EFI_FILE_PROTOCOL *FileRoot;
    status = FS->OpenVolume(FS, &FileRoot);
    SASSERT(status);

    EFI_FILE_PROTOCOL *FileTarget;
    status = FileRoot->Open(FileRoot,
                            &FileTarget,
                            TargetPath,
                            EFI_FILE_MODE_READ,
                            0);
    SASSERT(status);

    *BufferSize = 0;
    status = FileTarget->GetInfo(FileTarget,
                                 &gEfiFileInfoGuid,
                                 BufferSize,
                                 NULL);
    if (status != EFI_BUFFER_TOO_SMALL) {
        if (status == EFI_SUCCESS) { status = EFI_ABORTED; }
        SASSERT(status);
    }

    EFI_FILE_INFO *TargetFileInfo;
    status = gBS->AllocatePool(EfiLoaderData,
                               *BufferSize,
                               (VOID **)&TargetFileInfo);
    SASSERT(status);

    status = FileTarget->GetInfo(FileTarget,
                                 &gEfiFileInfoGuid,
                                 BufferSize,
                                 (VOID *)TargetFileInfo);
    SASSERT(status);

    *BufferSize = TargetFileInfo->FileSize;
    gBS->FreePool((VOID *)TargetFileInfo);
    TargetFileInfo = NULL;

    status = gBS->AllocatePool(EfiLoaderData,
                               *BufferSize,
                               Buffer);
    SASSERT(status);

    status = FileTarget->Read(FileTarget,
                              BufferSize,
                              *Buffer);
    SASSERT(status);

    return status;

Abort:
    return status;
}
