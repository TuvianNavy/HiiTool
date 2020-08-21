#include "VarCheckHiiGen.h"

int main(int argc, char **argv)
{
  EFI_HII_DATABASE_PROTOCOL *HiiDatabase;
  EFI_HII_CONFIG_ROUTING_PROTOCOL *HiiConfigRouting;
  EFI_STATUS efi_status;
  UINTN buflen;
  EFI_HII_PACKAGE_LIST_HEADER *HiiHandleBuffer;
  EFI_PHYSICAL_ADDRESS bufaddr;
  UINT16 buflen_keymaps;
  EFI_GUID *KeyMaps;
  EFI_PHYSICAL_ADDRESS bufaddr_keymaps;
  BOOLEAN no_keymap;

  efi_status = gBS->LocateProtocol (&gEfiHiiDatabaseProtocolGuid, NULL, (VOID **) &HiiDatabase);
  if (EFI_ERROR (efi_status)) { return 1; }
  efi_status = gBS->LocateProtocol (&gEfiHiiConfigRoutingProtocolGuid, NULL, (VOID **) &HiiConfigRouting);
  if (EFI_ERROR (efi_status)) { return 1; }

  buflen_keymaps = 0;
  no_keymap = 0;
  efi_status = HiiDatabase->FindKeyboardLayouts (
               HiiDatabase,
               &buflen_keymaps,
               NULL);
  if (efi_status == EFI_NOT_FOUND) {
    no_keymap = 1;
    printf ("found no keymap\n");
  } else if (efi_status != EFI_BUFFER_TOO_SMALL) {
    return 1;
  }
  if (!no_keymap) {
    efi_status = gBS->AllocatePages (AllocateAnyPages, EfiBootServicesData, EFI_SIZE_TO_PAGES(buflen_keymaps), &bufaddr_keymaps);
    if (EFI_ERROR (efi_status)) {
      return 1;
    }
    KeyMaps = (EFI_HII_HANDLE) bufaddr_keymaps;
    printf ("allocated 4K blocks larger than %d bytes\n", buflen_keymaps);
    efi_status = HiiDatabase->FindKeyboardLayouts (
                 HiiDatabase,
                 &buflen_keymaps,
                 KeyMaps);
    if (EFI_ERROR (efi_status)) {
      return 1;
    }
    printf ("found %d keyboard layout(s)\n", buflen_keymaps / 16);
    gBS->FreePages (bufaddr_keymaps, buflen_keymaps);
  }
  return 1;

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
  DumpHiiDatabase (HiiHandleBuffer, buflen);
/*
  FreePool (HiiHandleBuffer);
*/
  gBS->FreePages (bufaddr, buflen);
  printf("all OK\n");
  return 0;
}
