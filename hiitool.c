#include <stdio.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/UefiHiiServicesLib.h>
#include <Library/HiiLib.h>
#include <Guid/MdeModuleHii.h>

int main(int argc, char **argv)
{
  EFI_HII_DATABASE_PROTOCOL *HiiDatabase;
  EFI_STATUS efi_status;
  UINTN buflen;
  EFI_HII_HANDLE dummybufptr;
  EFI_HII_HANDLE *HiiHandleBuffer;

  efi_status = gBS->LocateProtocol (
               &gEfiHiiDatabaseProtocolGuid,
               NULL,
               (VOID **) &HiiDatabase);
  if (EFI_ERROR (efi_status)) {
    return 1;
  }
  efi_status = HiiDatabase->ListPackageLists (
               gHiiDatabase,
               EFI_HII_PACKAGE_SIMPLE_FONTS,
               NULL,
               &buflen,
               &dummybufptr);
  if (efi_status != EFI_BUFFER_TOO_SMALL) {
    return 1;
  }
  HiiHandleBuffer = AllocateZeroPool (buflen + sizeof (EFI_HII_HANDLE));
  if (HiiHandleBuffer == NULL) {
    return 1;
  }
  efi_status = HiiDatabase->ListPackageLists (
               gHiiDatabase,
               EFI_HII_PACKAGE_SIMPLE_FONTS,
               NULL,
               &buflen,
               HiiHandleBuffer); 
  if (EFI_ERROR (efi_status)) {
    FreePool (HiiHandleBuffer);
    return 1;
  }
  FreePool (HiiHandleBuffer);
  return 0;
}
