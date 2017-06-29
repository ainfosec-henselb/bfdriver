
#ifndef HYP_FILE_ACCESS_H
#define HYP_FILE_ACCESS_H

#include <Protocol/DevicePath.h>

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

#endif
