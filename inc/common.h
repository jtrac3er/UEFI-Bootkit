// diese includes werden von allen Teilen gebraucht

#ifndef __COMMON_H_EFI__
#define __COMMON_H_EFI__


// Debug Funktonalitäten aktivieren
//#define DEBUG_MAIN_EFI


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

#include <efi.h>
#include <efilib.h>

#define REMOVE_EFI_DOUBLE_INCLUDES
#include <Base.h>
#include <IndustryStandard/Acpi.h>

//#define PTR_WORD UINT64
#define PTR_WORD UINT32			// man verwendet 32 bit addressen!!

#ifdef DEBUG_MAIN_EFI
	#define DEBUG_PRINT(...) Print(__VA_ARGS__)
#else
	#define DEBUG_PRINT(...)
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)


#endif

