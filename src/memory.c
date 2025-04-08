
#include "memory.h"

EFI_STATUS get_memory_map(MEMORY_MAP_STRUCT *MemoryMapStruct) {

    EFI_STATUS Status;
    UINTN MemoryMapSize = 0;
    EFI_MEMORY_DESCRIPTOR *MemoryMap = NULL;
    UINTN MapKey, DescriptorSize;
    UINT32 DescriptorVersion;

    // Erhalte die Größe der Memory Map
    Status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5,
        &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);

    if (EFI_ERROR(Status) && Status != EFI_BUFFER_TOO_SMALL) {
        // Etwas ist schiefgelaufen
        Print(L"Fehler aufgetreten: Errorcode = %r\n", Status);
        return Status;
    }

    if (Status == EFI_BUFFER_TOO_SMALL) {
        // Allokiere Speicher für die Memory Map
        MemoryMap = AllocatePool(MemoryMapSize);
        if (MemoryMap == NULL) {
            Print(L"Fehler: Speicher für die Memory Map konnte nicht allocated werden.\n");
            return EFI_OUT_OF_RESOURCES;
        }

        // Rufe die Memory Map erneut ab
        MemoryMapSize += DescriptorSize*2;  // erhöhe Size, weil alloc die memory map splittet
        Status = uefi_call_wrapper(ST->BootServices->GetMemoryMap, 5,
            &MemoryMapSize, MemoryMap, &MapKey, &DescriptorSize, &DescriptorVersion);
        if (EFI_ERROR(Status)) {
            Print(L"Fehler beim Abrufen der Memory Map: %r\n", Status);
            FreePool(MemoryMap);
            return Status;
        }
    }

    MemoryMapStruct->MemoryMap = MemoryMap;
    MemoryMapStruct->MemoryMapSize = MemoryMapSize;
    MemoryMapStruct->MapKey = MapKey;
    MemoryMapStruct->DescriptorSize = DescriptorSize;
    MemoryMapStruct->DescriptorVersion = DescriptorVersion;

    return EFI_SUCCESS;
}


void print_memory_map(MEMORY_MAP_STRUCT MemoryMapStruct) {
    // Zeige die Memory Map an
    Print(L"Type       Physical Start     Virtual Start      Number of Pages  Attributes\n");

    UINTN EntryCount = MemoryMapStruct.MemoryMapSize / MemoryMapStruct.DescriptorSize;
    EFI_MEMORY_DESCRIPTOR *Descriptor = MemoryMapStruct.MemoryMap;
    for (UINTN i = 0; i < EntryCount; i++) {
        Print(L"%d          %lx              %lx              %lx             %lx\n",
              Descriptor->Type,
              Descriptor->PhysicalStart,
              Descriptor->VirtualStart,
              Descriptor->NumberOfPages,
              Descriptor->Attribute);
        
        // Nächster Eintrag
        Descriptor = (EFI_MEMORY_DESCRIPTOR *)((UINT8 *)Descriptor + MemoryMapStruct.DescriptorSize);
    }
}


void delete_memory_map(MEMORY_MAP_STRUCT MemoryMapStruct) { 
    // Speicher freigeben
    FreePool(MemoryMapStruct.MemoryMap);
}



EFI_STATUS hide_from_memory_map(UINTN noPages, void **address) {
    
    EFI_STATUS Status;

    Status = uefi_call_wrapper(ST->BootServices->AllocatePages, 4, 
        AllocateAnyPages, EfiUnusableMemory, noPages, address);
    if (EFI_ERROR(Status)) {
        Print(L"Konnte keinen Speicher allocaten, %r", Status);
    }
    
    return EFI_SUCCESS;
}

