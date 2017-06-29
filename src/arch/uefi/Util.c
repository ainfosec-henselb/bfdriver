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

#include <Driver.h>

CHAR16 *DEFAULT_HYPERVISOR_PATH = L"\\EFI\\hypervisor\\bfvmm_main";

EFI_STATUS
GetKeystroke (
    OUT EFI_INPUT_KEY *Key
)
{

    UINTN EventIndex;
    EFI_STATUS status;

    do {
        gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
        status = gST->ConIn->ReadKeyStroke(gST->ConIn, Key);
    } while (status == EFI_NOT_READY);

    return status;

}
