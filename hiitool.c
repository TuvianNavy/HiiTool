/* StdLib/Include */
#include <stdio.h>
/* MdePkg/Include */
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
/* MdeModulePkg/Include */
#include <Library/UefiHiiServicesLib.h>

int main(int argc, char **argv)
{
  EFI_HII_DATABASE_PROTOCOL *HiiDatabase;
  EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting;
  EFI_STATUS efi_status;
  UINTN buflen;
  EFI_HII_PACKAGE_LIST_HEADER *HiiHandleBuffer;
  EFI_PHYSICAL_ADDRESS bufaddr;

  efi_status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (efi_status)) { return 1; }
  efi_status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (efi_status)) { return 1; }

  buflen = 0;
  efi_status = HiiDatabase->ExportPackageLists (
               HiiDatabase,
               0,
               &buflen,
               NULL);
  if (efi_status != EFI_BUFFER_TOO_SMALL) {
    return 1;
  }
/*
  HiiHandleBuffer = AllocateZeroPool (buflen + sizeof (EFI_HII_HANDLE));
  if (HiiHandleBuffer == NULL) {
*/
  efi_status = gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(buflen), &bufaddr);
  if (EFI_ERROR (efi_status)) {
    return 1;
  }
  HiiHandleBuffer = (EFI_HII_HANDLE) bufaddr;
  printf ("allocated 4K blocks larger than %d bytes\n", buflen);
  efi_status = HiiDatabase->ExportPackageLists (
               HiiDatabase,
               0,
               &buflen,
               HiiHandleBuffer); 
  if (EFI_ERROR (efi_status)) {
/*
    FreePool (HiiHandleBuffer);
*/
    gBS->FreePages (bufaddr, buflen);
    return 1;
  }
/*
  FreePool (HiiHandleBuffer);
*/
  gBS->FreePages (bufaddr, buflen);
  printf("all OK\n");
  return 0;
}
