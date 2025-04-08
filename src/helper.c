
#include "helper.h"

// Kleine Helferfunktionen

void printGUID(GUID* guid) {
    Print(L"%08x-%04x-%04x-", 
           guid->Data1, 
           guid->Data2, 
           guid->Data3);

    for (int i = 0; i < 2; i++) {
        Print(L"%02x", guid->Data4[i]);
    }
    Print(L"-");
    for (int i = 2; i < 8; i++) {
        Print(L"%02x", guid->Data4[i]);
    }
    Print(L"\n");
}


extern char _text[];	// beginn .text
extern char _data[];	// beginn .data

void print_section_addresses() {
	Print(L"Section Addresses relocated:\n");
	Print(L".text @ 0x%lx\n.data @ 0x%lx\n", _text, _data);
}


