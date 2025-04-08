
#include "proc.h"


EFI_STATUS get_madt(MADT_TYPE **Madt) {

    EFI_STATUS Status;
    EFI_CONFIGURATION_TABLE *ConfigTable;
    EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER *Rsdp = NULL;
    EFI_ACPI_DESCRIPTION_HEADER *Rsdt = NULL;
    UINTN i;

    // Suche die ACPI RSDP in den UEFI-Konfigurationstabellen
    for (i = 0; i < ST->NumberOfTableEntries; i++) {
        if (CompareGuid(&ST->ConfigurationTable[i].VendorGuid, &(GUID)ACPI_TABLE_GUID) == 0x0) {
            Rsdp = (EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_POINTER *)ST->ConfigurationTable[i].VendorTable;
            break;
        }
    }

    if (!Rsdp) {
        Print(L"ACPI 1.0 RSDP nicht gefunden!\n");
        return EFI_NOT_FOUND;
    }

    // Lade die RSDT (32-Bit-Adresse)
    Rsdt = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Rsdp->RsdtAddress;
    if (!Rsdt || Rsdt->Signature != EFI_ACPI_1_0_ROOT_SYSTEM_DESCRIPTION_TABLE_SIGNATURE) {
        Print(L"RSDT nicht gefunden oder ungueltig!\n");
        return EFI_NOT_FOUND;
    }

    // Iteriere über die Tabellen-Einträge in der RSDT
    UINTN EntryCount = (Rsdt->Length - sizeof(EFI_ACPI_DESCRIPTION_HEADER)) / sizeof(UINT32);
    UINT32 *Entry = (UINT32 *)((UINT8 *)Rsdt + sizeof(EFI_ACPI_DESCRIPTION_HEADER));

    for (i = 0; i < EntryCount; i++) {
        EFI_ACPI_DESCRIPTION_HEADER *Header = (EFI_ACPI_DESCRIPTION_HEADER *)(UINTN)Entry[i];
        if (Header->Signature == EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE) {
            *Madt = (EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER *)Header;
            break;
        }
    }

    if (!(*Madt)) {
        Print(L"MADT nicht gefunden!\n");
        return EFI_NOT_FOUND;
    }

    return EFI_SUCCESS;

}


void print_madt(MADT_TYPE *Madt) {

    // Durchsuche die MADT-Einträge
    PTR_WORD APIC_Entry = (PTR_WORD)Madt + sizeof(EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
    PTR_WORD End = (PTR_WORD)Madt + Madt->Header.Length;

    while (APIC_Entry < End) {
        EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE *ProcessorEntry = (
        	EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE *)APIC_Entry;

        switch (ProcessorEntry->Type) {
            case EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC:
            	{
	                Print(L"  Processor Local APIC:\n");
	                Print(L"    ACPI Processor ID: %d\n", ProcessorEntry->AcpiProcessorId);
	                Print(L"    APIC ID: %d\n", ProcessorEntry->ApicId);
	                Print(L"    Flags: %s\n", (ProcessorEntry->Flags & EFI_ACPI_1_0_LOCAL_APIC_ENABLED) ? 
	                							L"Enabled" : L"Disabled");
            	}
                break;

            case EFI_ACPI_1_0_IO_APIC:
            	{
            		// in klammern, weil man darf innerhalb eines swtich statements eigentlich keine neuen
            		// variablen definieren, deshalb muss man sie in einen block tun, um die Deklaration nur
            		// für diesen Block zu machen - dumm
	                EFI_ACPI_1_0_IO_APIC_STRUCTURE *IoApicEntry = (EFI_ACPI_1_0_IO_APIC_STRUCTURE *)APIC_Entry;
	                Print(L"  IO APIC:\n");
	                Print(L"    IO APIC ID: %d\n", IoApicEntry->IoApicId);
	                Print(L"    IO APIC Address: 0x%x\n", IoApicEntry->IoApicAddress);
	                Print(L"    Global System Interrupt Base: %d\n", IoApicEntry->SystemVectorBase);
	            	}
                break;

            case EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE:
            	{
	                EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE *IsoEntry = (
	                	EFI_ACPI_1_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE *)APIC_Entry;
	                Print(L"  Interrupt Source Override:\n");
	                Print(L"    Bus: %d\n", IsoEntry->Bus);
	                Print(L"    Source: %d\n", IsoEntry->Source);
	                Print(L"    Global System Interrupt: %d\n", IsoEntry->GlobalSystemInterruptVector);
	                Print(L"    Flags: 0x%x\n", IsoEntry->Flags);
            	}
                break;

            default:
                Print(L"  Unbekannter MADT-Eintragstyp: %d\n", ProcessorEntry->Type);
                break;
        }

        APIC_Entry += ProcessorEntry->Length;
    }
}

#if defined(SOFT_DISABLE) + defined(HARD_DISABLE) != 1
    #error Genauer einer muss sein: entweder SOFT_DISABLE oder HARD_DISABLE
#endif


#ifdef SOFT_DISABLE

EFI_STATUS manipulate_madt(MADT_TYPE *Madt, UINTN noCPUs, APIC_TYPE *apic_entries[]) {

    // Durchsuche die MADT-Einträge
    PTR_WORD Madt_Entry = (PTR_WORD)Madt + sizeof(EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);
    PTR_WORD End = (PTR_WORD)Madt + Madt->Header.Length;
    LOCAL_APIC_STRUCTURE *ownLocalAPIC = Madt->LocalApicAddress;
    int i = 0;

    while (Madt_Entry < End) {
        EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE *ApicEntry = (
            EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE *)Madt_Entry;

        // sich selber nicht deaktiveren, da man den boot
        if (ApicEntry->Type != EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC)
            goto continue_loop;

        // Bootstrap prozessor auch aulassen, gibt sonst sicher probleme
        if (ApicEntry->ApicId == ownLocalAPIC->ID)
            goto continue_loop;

        // dann kann man beginnen, die APICs zu deaktivieren
        if (noCPUs-- > 0) {
            apic_entries[i++] = ApicEntry;
            ApicEntry->Flags &= ~EFI_ACPI_1_0_LOCAL_APIC_ENABLED;
        } else {
            break;
        }

continue_loop:
        Madt_Entry += ApicEntry->Length;
    }

    if (noCPUs > 0)
        return EFI_OUT_OF_RESOURCES;
    else
        return EFI_SUCCESS;

}

#endif



#ifdef HARD_DISABLE

EFI_STATUS manipulate_madt(MADT_TYPE *Madt, UINTN noCPUs, APIC_TYPE *apic_entries[]) {
    return EFI_UNSUPPORTED;
}

#endif

