

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

extern CHAR16* DEFAULT_HYPERVISOR_PATH;
extern EFI_HANDLE gImage;
extern EFI_MP_SERVICES_PROTOCOL* gMpServices;

// Generates a device path for a file with PATH on 
// the same volume we were loaded from
EFI_STATUS
GenerateFileDP(
  IN CHAR16* TargetPath,
  OUT EFI_DEVICE_PATH_PROTOCOL** FileDP
);


// Read a file from our volume.  Caller's duty to
// FreePool on *Buffer when finished with it
EFI_STATUS
ReadFile(
  IN CHAR16* TargetPath,
  OUT VOID** Buffer,
  OUT UINTN* BufferSize
);


EFI_STATUS
GetKeystroke (
  OUT EFI_INPUT_KEY* Key
);


#define SASSERT(status)                                           \
if(EFI_ERROR(status)) {                                           \
    Print(L"\n %a: %d: returned status %r\n", __FILENAME__, __LINE__, status); \
    goto Abort;                                                 \
}

extern const char* modules_array_name;

#endif
