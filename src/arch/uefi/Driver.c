
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
EFI_MP_SERVICES_PROTOCOL* gMpServices = NULL;


EFI_STATUS
EFIAPI
UefiMain (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE*  SystemTable
)
{
  // UINT32 Index;

  // Index = 0;
  Print(L"\nHypervisor Tester\n");
  common_init();

  EFI_STATUS status;
  gImage = ImageHandle;

  status = gBS->LocateProtocol(&gEfiMpServiceProtocolGuid,
                               NULL,
                               (VOID**)&gMpServices);
  SASSERT(status);

  Print(L"MpServiceProtocol found \n");

  //
  // Testing for whether TR resets on core switch -
  //   which appears to be the case
  //
  UINTN cpus;
  UINTN ecpus;
  status = gMpServices->GetNumberOfProcessors(gMpServices,
           &cpus,
           &ecpus);
  SASSERT(status);

  int i;
  for (i = 0; i < cpus; i++) {
    platform_set_affinity(i);
    EFI_PROCESSOR_INFORMATION pi;
    status = gMpServices->GetProcessorInfo(gMpServices,
                                           i,
                                           &pi);
    SASSERT(status);

    Print(L"Preparing ProcessorId: %d\n", pi.ProcessorId);
    PrepareProcessor();
    CheckProcessor();
  }

  for (i = 0; i < cpus; i++) {
    platform_set_affinity(i);
    EFI_PROCESSOR_INFORMATION pi;
    status = gMpServices->GetProcessorInfo(gMpServices,
                                           i,
                                           &pi);
    SASSERT(status);

    Print(L"Checking ProcessorId: %d\n", pi.ProcessorId);
    CheckProcessor();
  }
  platform_set_affinity(0);


  GetKeystroke(NULL);


  //
  // Add BF modules with bfelf_loader
  //
  add_hypervisor_modules();

  GetKeystroke(NULL);


  //
  // Load and relocate ELF modules
  //
  int64_t ret = common_load_vmm();
  if (ret < 0) {
    Print(L"common_load_vmm returned %a\n", ec_to_str(ret));
    goto Abort;
  }


  //
  // Set up TSS per VMX requirements
  //
  Print(L"Preparing processor...\n");

  PrepareProcessor();

  Print(L"Processor prepared.\n");


  //
  // Start VMM
  //
  Print(L"Starting VMM...\n");
  ret = common_start_vmm();

  if (ret < 0) {
    goto Abort;
  }

  Print(L"VMM started.");

  GetKeystroke(NULL);


  //
  // Load/Start next boot step
  //
  Print(L"Loading OS\n");
  EFI_DEVICE_PATH_PROTOCOL* TargetDP;
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
  CHAR16* ExitData;
  status = gBS->StartImage(TargetHandle,
                           &ExitDataSize,
                           &ExitData);
  SASSERT(status);

Abort:
  Print(L"Failed to start hypervisor.\n");
  GetKeystroke(NULL);
  return status;

}
