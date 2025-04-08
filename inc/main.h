#include "common.h"

#define MAX_INPUT_LENGTH 128
#define ALLOCATE_NO_PAGES 100
#define DISABLE_NO_CORES 2

#define CMD_PRINT_MMAP "pm"
#define CMD_PRINT_PROC "pp"
#define CMD_HIDE_MEM "dm"
#define CMD_DISABLE_PROC "dp"
#define CMD_PCI "pci"
#define CMD_EXIT "ex"


EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable);

