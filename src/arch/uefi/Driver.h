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

#ifndef HYP_DRIVER_H
#define HYP_DRIVER_H

#include <stdint.h>
#include <string.h>
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Pi/PiMultiPhase.h>
#include <Protocol/MpService.h>
#include <Protocol/DevicePath.h>

#ifdef __GNUC__
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

extern CHAR16 *DEFAULT_HYPERVISOR_PATH;
extern EFI_HANDLE gImage;
extern EFI_MP_SERVICES_PROTOCOL *gMpServices;

/**
 * Generate File Device Path
 *
 * Generates a device path for a file with TargetPath on
 * the same volume we were loaded from
 *
 * @param[in] TargetPath File to generate a device path for -
 *                       should start with a backslash and use 
 *                       backslashes throughout
 * @param[out] FileDP    Device path for file on volume this
 *                       image was started from
 *
 * @return EFI_SUCCESS if successful
 */
EFI_STATUS
GenerateFileDP(
    IN CHAR16 *TargetPath,
    OUT EFI_DEVICE_PATH_PROTOCOL **FileDP
);

/**
 * Read File from drive
 *
 * Read a file from our volume.  Caller's duty to
 * FreePool on *Buffer when finished with it
 *
 * @param[in]  TargetPath File to read into a buffer -
 *                        should start with a backslash and use 
 *                        backslashes throughout
 * @param[out] Buffer     Points *Buffer to a buffer containing 
 *                        file's contents
 * @param[out] BufferSize Length of output file buffer
 *
 * @return EFI_SUCCESS if successful
 */
EFI_STATUS
ReadFile(
    IN CHAR16 *TargetPath,
    OUT VOID **Buffer,
    OUT UINTN *BufferSize
);

/**
 * Get Keystroke
 *
 * Get a keystroke from the keyboard
 *
 * @return EFI_SUCCESS if successful
 */
EFI_STATUS
GetKeystroke (
    OUT EFI_INPUT_KEY *Key
);

#define SASSERT(status)                                                        \
if(EFI_ERROR(status)) {                                                        \
    Print(L"\n %a: %d: returned status %r\n", __FILENAME__, __LINE__, status); \
    goto Abort;                                                                \
}

extern const char *modules_array_name;

#endif
