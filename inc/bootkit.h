
#include "common.h"
#include <Protocol/MpService.h>

// so werden diese Funktionen in einer eigenen Sektion platziert, welche Page-alignt ist
// so kann diese Funktion auch sicher mit dem offset 0x0 aufgerufen werden
#define BOOTKIT_FUNC __attribute__((section(".bootkit"), aligned(0x1000)))
#define INTERNAL

typedef struct {	
	UINTN ProcessorId;
	EFI_SYSTEM_TABLE *SystemTable;
} PROCESSOR_ARGUMENTS;

void BOOTKIT_FUNC bootkit_entry();

void BOOTKIT_FUNC INTERNAL application_processor_entry(PROCESSOR_ARGUMENTS *args);