#include "main.h"
#include "proc.h"
#include "memory.h"
#include "pci.h"

EFI_STATUS EFIAPI efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {

    EFI_STATUS Status;
    CHAR16 InputBuffer[MAX_INPUT_LENGTH];
    UINTN InputSize;
    EFI_INPUT_KEY Key;
    UINTN Index;

    InitializeLib(ImageHandle, SystemTable);

    // für debugging
#ifdef DEBUG_MAIN_EFI
    print_section_addresses();
    Print(L"\n");
#endif

    // Begrüßungsnachricht
    Print(L"UEFI Command Interpreter:\n");
    Print(L"" CMD_PRINT_MMAP " - Printe die Memory Map\n");
    Print(L"" CMD_PRINT_PROC " - Printe die MADT Tablle\n");
    Print(L"" CMD_HIDE_MEM " - Allocate Speicher fuer die Malware (" STR(ALLOCATE_NO_PAGES) L" Pages)\n");
    Print(L"" CMD_DISABLE_PROC " - Deaktiviere CPU Kerne fuer die Malware (" STR(DISABLE_NO_CORES) L" Cores)\n");
    Print(L"" CMD_PCI "  - PCI Inspektion durchfuehren\n");
    Print(L"" CMD_EXIT "  - Beende dieses Programm\n");

    while (TRUE) {
        // Konsoleneingabeaufforderung
        Print(L"\n> ");
        InputSize = 0;
        SetMem(InputBuffer, sizeof(InputBuffer), 0);

        // Lesen von Zeichen aus der Konsole
        while (TRUE) {

            while ((Status = uefi_call_wrapper(ST->ConIn->ReadKeyStroke, 2,
            		ST->ConIn, &Key)) == EFI_NOT_READY);

            if (EFI_ERROR(Status)) {
                Print(L"Error reading keystroke: %r\n", Status);
                return Status;
            }

            // Zeilenumbruch beendet die Eingabe
            if (Key.UnicodeChar == L'\r') {
                InputBuffer[InputSize] = L'\0';
                Print(L"\n");
                break;
            }

            // Zeichen sammeln (sofern im Puffer Platz ist)
            if (InputSize < MAX_INPUT_LENGTH - 1) {
                InputBuffer[InputSize++] = Key.UnicodeChar;
                Print(L"%c", Key.UnicodeChar); // Echo des Zeichens
            }
        }

        // Fallunterscheidung basierdnd auf der Eingabe
        if (StrCmp(InputBuffer, L"" CMD_PRINT_MMAP) == 0) {
			MEMORY_MAP_STRUCT MemoryMapStruct = {0};

			Status = get_memory_map(&MemoryMapStruct);
			if (EFI_ERROR(Status)) {
				Print(L"Konnte Memory Map nicht laden, Fehler: %r", Status);
				return Status;
			} else {
				Print(L"Memory Map erfolgreich geladen, Adresse = 0x%lx", MemoryMapStruct.MemoryMap);
			}

			print_memory_map(MemoryMapStruct);
			delete_memory_map(MemoryMapStruct);



        } else if (StrCmp(InputBuffer, L"" CMD_PRINT_PROC) == 0) {
			MADT_TYPE *Madt = NULL;

			Status = get_madt(&Madt);
			if (EFI_ERROR(Status)) {
				Print(L"Konnte Memory MADT nicht laden, Fehler: %r", Status);
				return Status;
			} else {
				Print(L"MADT erfolgreich geladen, Adresse = 0x%lx", Madt);
			}

			print_madt(Madt);



        } else if (StrCmp(InputBuffer, L"" CMD_HIDE_MEM) == 0) {
        	void *address_first_page;

            Status = hide_from_memory_map(ALLOCATE_NO_PAGES, &address_first_page);
			if (EFI_ERROR(Status)) {
				Print(L"Konnte Pages nicht allocaten: %r", Status);
				return Status;
			} else {
				Print(L"Pages erfolgreich allocated @ 0x%lx", address_first_page);
			}



        } else if (StrCmp(InputBuffer, L"" CMD_DISABLE_PROC) == 0) {
			MADT_TYPE *Madt = NULL;
			APIC_TYPE *apicList[DISABLE_NO_CORES];

			Status = get_madt(&Madt);
			if (EFI_ERROR(Status)) {
				Print(L"Konnte Memory MADT nicht laden, Fehler: %r", Status);
				return Status;
			} else {
				Print(L"MADT erfolgreich geladen, Adresse = 0x%lx", Madt);
			}

			manipulate_madt(Madt, DISABLE_NO_CORES, apicList);



        } else if (StrCmp(InputBuffer, L"" CMD_PCI) == 0) {
            pci_inspection();



        } else if (StrCmp(InputBuffer, L"" CMD_EXIT) == 0) {
            return EFI_SUCCESS;



        } else {
            Print(L"Unknown command: %s\n", InputBuffer);
        }
    }

    return EFI_SUCCESS;
}
