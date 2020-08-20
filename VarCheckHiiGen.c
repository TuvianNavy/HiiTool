/** @file
  Var Check Hii bin generation.

Copyright (c) 2015 - 2018, Intel Corporation. All rights reserved.<BR>
SPDX-License-Identifier: BSD-2-Clause-Patent

**/

#include "VarCheckHiiGen.h"

LIST_ENTRY mVarCheckHiiList = INITIALIZE_LIST_HEAD_VARIABLE (mVarCheckHiiList);

#define VAR_CHECK_HII_VARIABLE_NODE_SIGNATURE   SIGNATURE_32 ('V', 'C', 'H', 'V')

typedef struct {
  UINTN                         Signature;
  LIST_ENTRY                    Link;
  VAR_CHECK_HII_VARIABLE_HEADER *HiiVariable;
  EFI_VARSTORE_ID               VarStoreId;

  VAR_CHECK_HII_QUESTION_HEADER **HiiQuestionArray;
} VAR_CHECK_HII_VARIABLE_NODE;

#define VAR_CHECK_HII_VARIABLE_FROM_LINK(a) CR (a, VAR_CHECK_HII_VARIABLE_NODE, Link, VAR_CHECK_HII_VARIABLE_NODE_SIGNATURE)

CHAR16 *mVarName = NULL;
UINTN  mMaxVarNameSize = 0;

#ifdef DUMP_HII_DATA
GLOBAL_REMOVE_IF_UNREFERENCED VAR_CHECK_HII_OPCODE_STRING   mIfrOpCodeStringTable[] = {
  {EFI_IFR_VARSTORE_OP,             "EFI_IFR_VARSTORE_OP"},
  {EFI_IFR_VARSTORE_EFI_OP,         "EFI_IFR_VARSTORE_EFI_OP"},
  {EFI_IFR_ONE_OF_OP,               "EFI_IFR_ONE_OF_OP"},
  {EFI_IFR_CHECKBOX_OP,             "EFI_IFR_CHECKBOX_OP"},
  {EFI_IFR_NUMERIC_OP,              "EFI_IFR_NUMERIC_OP"},
  {EFI_IFR_ORDERED_LIST_OP,         "EFI_IFR_ORDERED_LIST_OP"},
  {EFI_IFR_ONE_OF_OPTION_OP,        "EFI_IFR_ONE_OF_OPTION_OP"},
};

/**
  Ifr opcode to string.

  @param[in] IfrOpCode  Ifr OpCode.

  @return Pointer to string.

**/
CHAR8 *
IfrOpCodeToStr (
  IN UINT8  IfrOpCode
  )
{
  UINTN  Index;
  for (Index = 0; Index < ARRAY_SIZE (mIfrOpCodeStringTable); Index++) {
    if (mIfrOpCodeStringTable[Index].HiiOpCode == IfrOpCode) {
      return mIfrOpCodeStringTable[Index].HiiOpCodeStr;
    }
  }

  return "<UnknownIfrOpCode>";
}

GLOBAL_REMOVE_IF_UNREFERENCED VAR_CHECK_HII_PACKAGE_TYPE_STRING  mPackageTypeStringTable[] = {
  {EFI_HII_PACKAGE_TYPE_ALL,            "EFI_HII_PACKAGE_TYPE_ALL"},
  {EFI_HII_PACKAGE_TYPE_GUID,           "EFI_HII_PACKAGE_TYPE_GUID"},
  {EFI_HII_PACKAGE_FORMS,               "EFI_HII_PACKAGE_FORMS"},
  {EFI_HII_PACKAGE_STRINGS,             "EFI_HII_PACKAGE_STRINGS"},
  {EFI_HII_PACKAGE_FONTS,               "EFI_HII_PACKAGE_FONTS"},
  {EFI_HII_PACKAGE_IMAGES,              "EFI_HII_PACKAGE_IMAGES"},
  {EFI_HII_PACKAGE_SIMPLE_FONTS,        "EFI_HII_PACKAGE_SIMPLE_FONTS"},
  {EFI_HII_PACKAGE_DEVICE_PATH,         "EFI_HII_PACKAGE_DEVICE_PATH"},
  {EFI_HII_PACKAGE_KEYBOARD_LAYOUT,     "EFI_HII_PACKAGE_KEYBOARD_LAYOUT"},
  {EFI_HII_PACKAGE_ANIMATIONS,          "EFI_HII_PACKAGE_ANIMATIONS"},
  {EFI_HII_PACKAGE_END,                 "EFI_HII_PACKAGE_END"},
  {EFI_HII_PACKAGE_TYPE_SYSTEM_BEGIN,   "EFI_HII_PACKAGE_TYPE_SYSTEM_BEGIN"},
  {EFI_HII_PACKAGE_TYPE_SYSTEM_END,     "EFI_HII_PACKAGE_TYPE_SYSTEM_END"},
};

/**
  Hii Package type to string.

  @param[in] PackageType    Package Type

  @return Pointer to string.

**/
CHAR8 *
HiiPackageTypeToStr (
  IN UINT8  PackageType
  )
{
  UINTN     Index;
  for (Index = 0; Index < ARRAY_SIZE (mPackageTypeStringTable); Index++) {
    if (mPackageTypeStringTable[Index].PackageType == PackageType) {
      return mPackageTypeStringTable[Index].PackageTypeStr;
    }
  }

  return "<UnknownPackageType>";
}

/**
  Dump Hii Package.

  @param[in] HiiPackage         Pointer to Hii Package.

**/
VOID
DumpHiiPackage (
  IN VOID       *HiiPackage
  )
{
  EFI_HII_PACKAGE_HEADER        *HiiPackageHeader;
  EFI_IFR_OP_HEADER             *IfrOpCodeHeader;
  EFI_IFR_VARSTORE              *IfrVarStore;
  EFI_IFR_VARSTORE_EFI          *IfrEfiVarStore;
  BOOLEAN                       QuestionStoredInBitField;

  HiiPackageHeader = (EFI_HII_PACKAGE_HEADER *) HiiPackage;
  QuestionStoredInBitField = FALSE;

  DEBUG ((DEBUG_INFO, "  HiiPackageHeader->Type   - 0x%02x (%a)\n", HiiPackageHeader->Type, HiiPackageTypeToStr ((UINT8) HiiPackageHeader->Type)));
  DEBUG ((DEBUG_INFO, "  HiiPackageHeader->Length - 0x%06x\n", HiiPackageHeader->Length));

}

/**
  Dump Hii Database.

  @param[in] HiiDatabase        Pointer to Hii Database.
  @param[in] HiiDatabaseSize    Hii Database size.

**/
VOID
DumpHiiDatabase (
  IN VOID       *HiiDatabase,
  IN UINTN      HiiDatabaseSize
  )
{
  EFI_HII_PACKAGE_LIST_HEADER   *HiiPackageListHeader;
  EFI_HII_PACKAGE_HEADER        *HiiPackageHeader;

  DEBUG ((DEBUG_INFO, "HiiDatabaseSize - 0x%x\n", HiiDatabaseSize));
  HiiPackageListHeader = (EFI_HII_PACKAGE_LIST_HEADER *) HiiDatabase;

  while ((UINTN) HiiPackageListHeader < ((UINTN) HiiDatabase + HiiDatabaseSize)) {
    DEBUG ((DEBUG_INFO, "HiiPackageListHeader->PackageListGuid - %g\n", &HiiPackageListHeader->PackageListGuid));
    DEBUG ((DEBUG_INFO, "HiiPackageListHeader->PackageLength   - 0x%x\n", (UINTN)HiiPackageListHeader->PackageLength));
    HiiPackageHeader = (EFI_HII_PACKAGE_HEADER *)(HiiPackageListHeader + 1);

    while ((UINTN) HiiPackageHeader < (UINTN) HiiPackageListHeader + HiiPackageListHeader->PackageLength) {

      DumpHiiPackage (HiiPackageHeader);

      HiiPackageHeader = (EFI_HII_PACKAGE_HEADER *) ((UINTN) HiiPackageHeader + HiiPackageHeader->Length);
    }

    HiiPackageListHeader = (EFI_HII_PACKAGE_LIST_HEADER *) ((UINTN) HiiPackageListHeader + HiiPackageListHeader->PackageLength);
  }

  return ;
}
#endif

/**
  Allocates a buffer of a certain pool type.

  Allocates the number bytes specified by AllocationSize of a certain pool type and returns a
  pointer to the allocated buffer.  If AllocationSize is 0, then a valid buffer of 0 size is
  returned.  If there is not enough memory remaining to satisfy the request, then NULL is returned.

  @param  MemoryType            The type of memory to allocate.
  @param  AllocationSize        The number of bytes to allocate.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
InternalVarCheckAllocatePool (
  IN EFI_MEMORY_TYPE  MemoryType,
  IN UINTN            AllocationSize
  )
{
  EFI_STATUS  Status;
  VOID        *Memory;

  Status = gBS->AllocatePool (MemoryType, AllocationSize, &Memory);
  if (EFI_ERROR (Status)) {
    Memory = NULL;
  }
  return Memory;
}

/**
  Allocates and zeros a buffer of type EfiBootServicesData.

  Allocates the number bytes specified by AllocationSize of type EfiBootServicesData, clears the
  buffer with zeros, and returns a pointer to the allocated buffer.  If AllocationSize is 0, then a
  valid buffer of 0 size is returned.  If there is not enough memory remaining to satisfy the
  request, then NULL is returned.

  @param  AllocationSize        The number of bytes to allocate and zero.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
InternalVarCheckAllocateZeroPool (
  IN UINTN            AllocationSize
  )
{
  VOID  *Memory;

  Memory = InternalVarCheckAllocatePool (EfiBootServicesData, AllocationSize);
  if (Memory != NULL) {
    Memory = ZeroMem (Memory, AllocationSize);
  }
  return Memory;
}

/**
  Frees a buffer that was previously allocated with one of the pool allocation functions in the
  Memory Allocation Library.

  Frees the buffer specified by Buffer.  Buffer must have been allocated on a previous call to the
  pool allocation services of the Memory Allocation Library.  If it is not possible to free pool
  resources, then this function will perform no actions.

  If Buffer was not allocated with a pool allocation function in the Memory Allocation Library,
  then ASSERT().

  @param  Buffer                The pointer to the buffer to free.

**/
VOID
EFIAPI
InternalVarCheckFreePool (
  IN VOID   *Buffer
  )
{
  EFI_STATUS    Status;

  Status = gBS->FreePool (Buffer);
  ASSERT_EFI_ERROR (Status);
}

/**
  Reallocates a buffer of type EfiBootServicesData.

  Allocates and zeros the number bytes specified by NewSize from memory of type
  EfiBootServicesData.  If OldBuffer is not NULL, then the smaller of OldSize and
  NewSize bytes are copied from OldBuffer to the newly allocated buffer, and
  OldBuffer is freed.  A pointer to the newly allocated buffer is returned.
  If NewSize is 0, then a valid buffer of 0 size is  returned.  If there is not
  enough memory remaining to satisfy the request, then NULL is returned.

  If the allocation of the new buffer is successful and the smaller of NewSize and OldSize
  is greater than (MAX_ADDRESS - OldBuffer + 1), then ASSERT().

  @param  OldSize        The size, in bytes, of OldBuffer.
  @param  NewSize        The size, in bytes, of the buffer to reallocate.
  @param  OldBuffer      The buffer to copy to the allocated buffer.  This is an optional
                         parameter that may be NULL.

  @return A pointer to the allocated buffer or NULL if allocation fails.

**/
VOID *
InternalVarCheckReallocatePool (
  IN UINTN            OldSize,
  IN UINTN            NewSize,
  IN VOID             *OldBuffer  OPTIONAL
  )
{
  VOID  *NewBuffer;

  NewBuffer = InternalVarCheckAllocateZeroPool (NewSize);
  if (NewBuffer != NULL && OldBuffer != NULL) {
    CopyMem (NewBuffer, OldBuffer, MIN (OldSize, NewSize));
    InternalVarCheckFreePool (OldBuffer);
  }
  return NewBuffer;
}

/**
  Get OneOf option data.

  @param[in]  IfrOpCodeHeader   Pointer to Ifr OpCode header.
  @param[out] Count             Pointer to option count.
  @param[out] Width             Pointer to option width.
  @param[out] OptionBuffer      Pointer to option buffer.

**/
VOID
GetOneOfOption (
  IN  EFI_IFR_OP_HEADER     *IfrOpCodeHeader,
  OUT UINTN                 *Count,
  OUT UINT8                 *Width,
  OUT VOID                  *OptionBuffer OPTIONAL
  )
{
  UINTN                     Scope;
  EFI_IFR_ONE_OF_OPTION     *IfrOneOfOption;

  //
  // Assume all OPTION has same Width.
  //
  *Count = 0;

  if (IfrOpCodeHeader->Scope != 0) {
    //
    // Nested OpCode.
    //
    Scope = 1;
    IfrOpCodeHeader = (EFI_IFR_OP_HEADER *) ((UINTN) IfrOpCodeHeader + IfrOpCodeHeader->Length);
    while (Scope != 0) {
      switch (IfrOpCodeHeader->OpCode) {
        case EFI_IFR_ONE_OF_OPTION_OP:
          IfrOneOfOption = (EFI_IFR_ONE_OF_OPTION *) IfrOpCodeHeader;
          switch (IfrOneOfOption->Type) {
            case EFI_IFR_TYPE_NUM_SIZE_8:
              *Count = *Count + 1;
              *Width = sizeof (UINT8);
              if (OptionBuffer != NULL) {
                CopyMem (OptionBuffer, &IfrOneOfOption->Value.u8, sizeof (UINT8));
                OptionBuffer = (UINT8 *) OptionBuffer + 1;
              }
              break;
            case EFI_IFR_TYPE_NUM_SIZE_16:
              *Count = *Count + 1;
              *Width = sizeof (UINT16);
              if (OptionBuffer != NULL) {
                CopyMem (OptionBuffer, &IfrOneOfOption->Value.u16, sizeof (UINT16));
                OptionBuffer = (UINT16 *) OptionBuffer + 1;
              }
              break;
            case EFI_IFR_TYPE_NUM_SIZE_32:
              *Count = *Count + 1;
              *Width = sizeof (UINT32);
              if (OptionBuffer != NULL) {
                CopyMem (OptionBuffer, &IfrOneOfOption->Value.u32, sizeof (UINT32));
                OptionBuffer = (UINT32 *) OptionBuffer + 1;
              }
              break;
            case EFI_IFR_TYPE_NUM_SIZE_64:
              *Count = *Count + 1;
              *Width = sizeof (UINT64);
              if (OptionBuffer != NULL) {
                CopyMem (OptionBuffer, &IfrOneOfOption->Value.u64, sizeof (UINT64));
                OptionBuffer = (UINT64 *) OptionBuffer + 1;
              }
              break;
            case EFI_IFR_TYPE_BOOLEAN:
              *Count = *Count + 1;
              *Width = sizeof (BOOLEAN);
              if (OptionBuffer != NULL) {
                CopyMem (OptionBuffer, &IfrOneOfOption->Value.b, sizeof (BOOLEAN));
                OptionBuffer = (BOOLEAN *) OptionBuffer + 1;
              }
              break;
            default:
              break;
          }
          break;
      }

      //
      // Until End OpCode.
      //
      if (IfrOpCodeHeader->OpCode == EFI_IFR_END_OP) {
        ASSERT (Scope > 0);
        Scope--;
        if (Scope == 0) {
          break;
        }
      } else if (IfrOpCodeHeader->Scope != 0) {
        //
        // Nested OpCode.
        //
        Scope++;
      }
      IfrOpCodeHeader = (EFI_IFR_OP_HEADER *) ((UINTN) IfrOpCodeHeader + IfrOpCodeHeader->Length);
    }
  }

  return ;
}

/**
  Parse Hii Question CheckBox.

  @param[in] IfrOpCodeHeader    Pointer to Ifr OpCode header.
  @param[in] StoredInBitField   Whether the CheckBox is stored in bit field Storage.

  return Pointer to Hii Question.

**/
VAR_CHECK_HII_QUESTION_HEADER *
ParseHiiQuestionCheckBox (
  IN EFI_IFR_OP_HEADER  *IfrOpCodeHeader,
  IN BOOLEAN            StoredInBitField
  )
{
  EFI_IFR_CHECKBOX                  *IfrCheckBox;
  VAR_CHECK_HII_QUESTION_CHECKBOX   *CheckBox;

  IfrCheckBox = (EFI_IFR_CHECKBOX *) IfrOpCodeHeader;

  CheckBox = InternalVarCheckAllocateZeroPool (sizeof (*CheckBox));
  ASSERT (CheckBox != NULL);
  CheckBox->OpCode         = EFI_IFR_CHECKBOX_OP;
  CheckBox->Length         = (UINT8) sizeof (*CheckBox);;
  CheckBox->VarOffset      = IfrCheckBox->Question.VarStoreInfo.VarOffset;
  CheckBox->BitFieldStore  = StoredInBitField;
  if (StoredInBitField) {
    CheckBox->StorageWidth = 1;
  } else {
    CheckBox->StorageWidth = (UINT8) sizeof (BOOLEAN);
  }

  return (VAR_CHECK_HII_QUESTION_HEADER *) CheckBox;
}

/**
  Parse Hii Question OrderedList.

  @param[in] IfrOpCodeHeader    Pointer to Ifr OpCode header.

  return Pointer to Hii Question.

**/
VAR_CHECK_HII_QUESTION_HEADER *
ParseHiiQuestionOrderedList (
  IN EFI_IFR_OP_HEADER  *IfrOpCodeHeader
  )
{
  EFI_IFR_ORDERED_LIST                  *IfrOrderedList;
  VAR_CHECK_HII_QUESTION_ORDEREDLIST    *OrderedList;
  UINTN                                 Length;
  UINTN                                 OptionCount;
  UINT8                                 OptionWidth;

  IfrOrderedList = (EFI_IFR_ORDERED_LIST *) IfrOpCodeHeader;

  GetOneOfOption (IfrOpCodeHeader, &OptionCount, &OptionWidth, NULL);

  Length = sizeof (*OrderedList) + OptionCount * OptionWidth;

  OrderedList = InternalVarCheckAllocateZeroPool (Length);
  ASSERT (OrderedList != NULL);
  OrderedList->OpCode        = EFI_IFR_ORDERED_LIST_OP;
  OrderedList->Length        = (UINT8) Length;
  OrderedList->VarOffset     = IfrOrderedList->Question.VarStoreInfo.VarOffset;
  OrderedList->StorageWidth  = OptionWidth;
  OrderedList->MaxContainers = IfrOrderedList->MaxContainers;
  OrderedList->BitFieldStore = FALSE;

  GetOneOfOption (IfrOpCodeHeader, &OptionCount, &OptionWidth, OrderedList + 1);

  return (VAR_CHECK_HII_QUESTION_HEADER *) OrderedList;
}


/**
  Find Hii variable node by name and GUID.

  @param[in] Name   Pointer to variable name.
  @param[in] Guid   Pointer to vendor GUID.

  @return Pointer to Hii Variable node.

**/
VAR_CHECK_HII_VARIABLE_NODE *
FindHiiVariableNode (
  IN CHAR16     *Name,
  IN EFI_GUID   *Guid
  )
{
  VAR_CHECK_HII_VARIABLE_NODE   *HiiVariableNode;
  LIST_ENTRY                    *Link;

  for (Link = mVarCheckHiiList.ForwardLink
      ;Link != &mVarCheckHiiList
      ;Link = Link->ForwardLink) {
    HiiVariableNode = VAR_CHECK_HII_VARIABLE_FROM_LINK (Link);

    if ((StrCmp (Name, (CHAR16 *) (HiiVariableNode->HiiVariable + 1)) == 0) &&
        CompareGuid (Guid, &HiiVariableNode->HiiVariable->Guid)) {
      return HiiVariableNode;
    }
  }

  return NULL;
}

/**
  Find Hii variable node by var store id.

  @param[in] VarStoreId         Var store id.

  @return Pointer to Hii Variable node.

**/
VAR_CHECK_HII_VARIABLE_NODE *
FindHiiVariableNodeByVarStoreId (
  IN EFI_VARSTORE_ID            VarStoreId
  )
{
  VAR_CHECK_HII_VARIABLE_NODE   *HiiVariableNode;
  LIST_ENTRY                    *Link;

  if (VarStoreId == 0) {
    //
    // The variable store identifier, which is unique within the current form set.
    // A value of zero is invalid.
    //
    return NULL;
  }

  for (Link = mVarCheckHiiList.ForwardLink
      ;Link != &mVarCheckHiiList
      ;Link = Link->ForwardLink) {
    HiiVariableNode = VAR_CHECK_HII_VARIABLE_FROM_LINK (Link);
    //
    // The variable store identifier, which is unique within the current form set.
    //
    if (VarStoreId == HiiVariableNode->VarStoreId) {
      return HiiVariableNode;
    }
  }

  return NULL;
}

/**
  Destroy var store id in the Hii Variable node after parsing one Hii Package.

**/
VOID
DestroyVarStoreId (
  VOID
  )
{
  VAR_CHECK_HII_VARIABLE_NODE   *HiiVariableNode;
  LIST_ENTRY                    *Link;

  for (Link = mVarCheckHiiList.ForwardLink
      ;Link != &mVarCheckHiiList
      ;Link = Link->ForwardLink) {
    HiiVariableNode = VAR_CHECK_HII_VARIABLE_FROM_LINK (Link);
    //
    // The variable store identifier, which is unique within the current form set.
    // A value of zero is invalid.
    //
    HiiVariableNode->VarStoreId = 0;
  }
}

/**
  Create Hii Variable node.

  @param[in] IfrEfiVarStore  Pointer to EFI VARSTORE.

**/
VOID
CreateHiiVariableNode (
  IN EFI_IFR_VARSTORE_EFI   *IfrEfiVarStore
  )
{
  VAR_CHECK_HII_VARIABLE_NODE   *HiiVariableNode;
  VAR_CHECK_HII_VARIABLE_HEADER *HiiVariable;
  UINTN                         HeaderLength;
  CHAR16                        *VarName;
  UINTN                         VarNameSize;

  //
  // Get variable name.
  //
  VarNameSize = AsciiStrSize ((CHAR8 *) IfrEfiVarStore->Name) * sizeof (CHAR16);
  if (VarNameSize > mMaxVarNameSize) {
    mVarName = InternalVarCheckReallocatePool (mMaxVarNameSize, VarNameSize, mVarName);
    ASSERT (mVarName != NULL);
    mMaxVarNameSize = VarNameSize;
  }
  AsciiStrToUnicodeStrS ((CHAR8 *) IfrEfiVarStore->Name, mVarName, mMaxVarNameSize / sizeof (CHAR16));
  VarName = mVarName;

  HiiVariableNode = FindHiiVariableNode (
                      VarName,
                      &IfrEfiVarStore->Guid
                      );
  if (HiiVariableNode == NULL) {
    //
    // Not found, then create new.
    //
    HeaderLength = sizeof (*HiiVariable) + VarNameSize;
    HiiVariable = InternalVarCheckAllocateZeroPool (HeaderLength);
    ASSERT (HiiVariable != NULL);
    HiiVariable->Revision = VAR_CHECK_HII_REVISION;
    HiiVariable->OpCode = EFI_IFR_VARSTORE_EFI_OP;
    HiiVariable->HeaderLength = (UINT16) HeaderLength;
    HiiVariable->Size = IfrEfiVarStore->Size;
    HiiVariable->Attributes = IfrEfiVarStore->Attributes;
    CopyGuid (&HiiVariable->Guid, &IfrEfiVarStore->Guid);
    StrCpyS ((CHAR16 *) (HiiVariable + 1), VarNameSize / sizeof (CHAR16), VarName);

    HiiVariableNode = InternalVarCheckAllocateZeroPool (sizeof (*HiiVariableNode));
    ASSERT (HiiVariableNode != NULL);
    HiiVariableNode->Signature = VAR_CHECK_HII_VARIABLE_NODE_SIGNATURE;
    HiiVariableNode->HiiVariable = HiiVariable;
    //
    // The variable store identifier, which is unique within the current form set.
    //
    HiiVariableNode->VarStoreId = IfrEfiVarStore->VarStoreId;
    HiiVariableNode->HiiQuestionArray = InternalVarCheckAllocateZeroPool (IfrEfiVarStore->Size * 8 * sizeof (VAR_CHECK_HII_QUESTION_HEADER *));

    InsertTailList (&mVarCheckHiiList, &HiiVariableNode->Link);
  } else {
    HiiVariableNode->VarStoreId = IfrEfiVarStore->VarStoreId;
  }
}

/**
  Parse and create Hii Variable node list.

  @param[in] HiiPackage         Pointer to Hii Package.

**/
VOID
ParseHiiVariable (
  IN VOID       *HiiPackage
  )
{
  EFI_HII_PACKAGE_HEADER    *HiiPackageHeader;
  EFI_IFR_OP_HEADER         *IfrOpCodeHeader;
  EFI_IFR_VARSTORE_EFI      *IfrEfiVarStore;

  HiiPackageHeader = (EFI_HII_PACKAGE_HEADER *) HiiPackage;

  switch (HiiPackageHeader->Type) {
    case EFI_HII_PACKAGE_FORMS:
      IfrOpCodeHeader = (EFI_IFR_OP_HEADER *) (HiiPackageHeader + 1);

      while ((UINTN) IfrOpCodeHeader < (UINTN) HiiPackageHeader + HiiPackageHeader->Length) {
        switch (IfrOpCodeHeader->OpCode) {
          case EFI_IFR_VARSTORE_EFI_OP:
            //
            // Come to EFI VARSTORE in Form Package.
            //
            IfrEfiVarStore = (EFI_IFR_VARSTORE_EFI *) IfrOpCodeHeader;
            if ((IfrEfiVarStore->Header.Length >= sizeof (EFI_IFR_VARSTORE_EFI)) &&
                ((IfrEfiVarStore->Attributes & EFI_VARIABLE_NON_VOLATILE) != 0)) {
              //
              // Only create node list for Hii Variable with NV attribute.
              //
              CreateHiiVariableNode (IfrEfiVarStore);
            }
            break;

          default:
            break;
        }
      IfrOpCodeHeader = (EFI_IFR_OP_HEADER *) ((UINTN) IfrOpCodeHeader + IfrOpCodeHeader->Length);
      }
      break;

    default:
      break;
  }
}

/**
  Var Check Parse Hii Package.

  @param[in] HiiPackage         Pointer to Hii Package.
  @param[in] FromFv             Hii Package from FV.

**/
VOID
VarCheckParseHiiPackage (
  IN VOID       *HiiPackage,
  IN BOOLEAN    FromFv
  )
{

  //
  // Parse and create Hii Variable node list for this Hii Package.
  //
  ParseHiiVariable (HiiPackage);

  DestroyVarStoreId ();
}

/**
  Var Check Parse Hii Database.

  @param[in] HiiDatabase        Pointer to Hii Database.
  @param[in] HiiDatabaseSize    Hii Database size.

**/
VOID
VarCheckParseHiiDatabase (
  IN VOID       *HiiDatabase,
  IN UINTN      HiiDatabaseSize
  )
{
  EFI_HII_PACKAGE_LIST_HEADER   *HiiPackageListHeader;
  EFI_HII_PACKAGE_HEADER        *HiiPackageHeader;

  HiiPackageListHeader = (EFI_HII_PACKAGE_LIST_HEADER *) HiiDatabase;

  while ((UINTN) HiiPackageListHeader < ((UINTN) HiiDatabase + HiiDatabaseSize)) {
    HiiPackageHeader = (EFI_HII_PACKAGE_HEADER *) (HiiPackageListHeader + 1);

    while ((UINTN) HiiPackageHeader < ((UINTN) HiiPackageListHeader + HiiPackageListHeader->PackageLength)) {
      //
      // Parse Hii Package.
      //
      VarCheckParseHiiPackage (HiiPackageHeader, FALSE);

      HiiPackageHeader = (EFI_HII_PACKAGE_HEADER *) ((UINTN) HiiPackageHeader + HiiPackageHeader->Length);
    }

    HiiPackageListHeader = (EFI_HII_PACKAGE_LIST_HEADER *) ((UINTN) HiiPackageListHeader + HiiPackageListHeader->PackageLength);
  }
}

