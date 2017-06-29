
#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include "x86_64.h"

#define KGDT64_SYS_TSS          0x60
#define AMD64_TSS               9


EFI_STATUS
CheckProcessor()
{
// EFLAGS bit 18 == alignment check
  _writeeflags(_readeflags() & ~(1<<18));

   PKGDTENTRY64 TssEntry;
  KDESCRIPTOR Gdtr;

  _sgdt((void*)&Gdtr.Limit);

  void* Gdt = (void*)Gdtr.Base;
  TssEntry = (PKGDTENTRY64)((uintptr_t)Gdt + KGDT64_SYS_TSS);

  Print(L"BaseMiddle: %x, Bits.Type: %x, Bits.Present: %x\n",
    TssEntry->Bits.BaseMiddle, TssEntry->Bits.Type, TssEntry->Bits.Present);

  uint16_t tester = _str();
  Print(L"TR: %x\n", tester);

  return EFI_SUCCESS;
}


EFI_STATUS
PrepareProcessor()
{
  // EFLAGS bit 18 == alignment check
  _writeeflags(_readeflags() & ~(1<<18));

  PKGDTENTRY64 TssEntry, NewGdt;
  PKTSS64 Tss;
  KDESCRIPTOR Gdtr;

  _sgdt((void*)&Gdtr.Limit);
  Print(L"Gdtr.Base: %x Gdtr.Limit: %x\n", Gdtr.Base, Gdtr.Limit);

  NewGdt = (PKGDTENTRY64)AllocateRuntimeZeroPool(MAX(Gdtr.Limit + 1,
                        KGDT64_SYS_TSS + sizeof(*TssEntry)));
  if (NewGdt == NULL)
  {
    return EFI_OUT_OF_RESOURCES;
  }

  gBS->CopyMem(NewGdt, Gdtr.Base, Gdtr.Limit + 1);

  Tss = AllocateRuntimeZeroPool(sizeof(*Tss)*2);
  if (Tss == NULL)
  {
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
  _lgdt((void*)&Gdtr.Limit);

  Print(L"GDT loaded\n");
  _sgdt((void*)&Gdtr.Limit);
  Print(L"new Gdtr.Base: %x  Gdtr.Limit: %x\n", Gdtr.Base, Gdtr.Limit);

  uint16_t tester = _str();
  Print(L"tester: %x\n", tester);
  _ltr(KGDT64_SYS_TSS);

  tester = _str();
  Print(L"new tester: %x\n", tester);

  return EFI_SUCCESS;
}

