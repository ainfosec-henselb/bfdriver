

#include <Driver.h>

CHAR16* DEFAULT_HYPERVISOR_PATH = L"\\EFI\\hypervisor\\bfvmm_main";

EFI_STATUS
GetKeystroke (
  OUT EFI_INPUT_KEY* Key
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
