#include "pci.h"

#define PCI_CONFIG_ADDRESS 0xCF8
#define PCI_CONFIG_DATA    0xCFC

// Funktion zum Lesen eines PCI-Registers
uint32_t pci_read_register(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset) {
    uint32_t address = (uint32_t)(
    	(bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(PCI_CONFIG_ADDRESS, address);
    return inl(PCI_CONFIG_DATA);
}

// Funktion zum Schreiben in ein PCI-Register
void pci_write_register(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t value) {
    uint32_t address = (uint32_t)(
    	(bus << 16) | (slot << 11) | (function << 8) | (offset & 0xFC) | ((uint32_t)0x80000000));
    outl(PCI_CONFIG_ADDRESS, address);
    outl(PCI_CONFIG_DATA, value);
}


// für ein Paar inspektionen aus
void pci_inspection() {

	pci_read_register(0,0,0)
}