
#include "common.h"


typedef struct {          
	UINTN MemoryMapSize;
	EFI_MEMORY_DESCRIPTOR *MemoryMap;
	UINTN MapKey;
	UINTN DescriptorSize;
	UINT32 DescriptorVersion;
} MEMORY_MAP_STRUCT;



// Die memory map (neu) erzeugen lassen von der Firmware
EFI_STATUS get_memory_map(MEMORY_MAP_STRUCT *MemoryMapStruct);	

// Memory map löschen - nur wenn vorher mit get_memory_map() erzeugt wurde!
void delete_memory_map(MEMORY_MAP_STRUCT MemoryMapStruct);

// Die Memory Map printen, nachdem man sie geladen hat
void print_memory_map(MEMORY_MAP_STRUCT MemoryMapStruct);		

// die Memory map manipulieren und eine gewisse Anzahl pages löschen
EFI_STATUS hide_from_memory_map(UINTN noPages, void **address);

