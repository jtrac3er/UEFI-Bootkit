#include "common.h"

#include <stdint.h>
#include <stdio.h>


// ChatGPT generiertes Struct

typedef struct {
    // Offsets 0x00 - 0x03
    uint16_t vendor_id;      // Vendor ID
    uint16_t device_id;      // Device ID

    // Offsets 0x04 - 0x07
    uint16_t command;        // Command register
    uint16_t status;         // Status register

    // Offsets 0x08 - 0x0B
    uint8_t revision_id;     // Revision ID
    uint8_t prog_if;         // Programming Interface
    uint8_t subclass;        // Subclass
    uint8_t class_code;      // Class Code

    // Offsets 0x0C - 0x0F
    uint8_t cache_line_size; // Cache Line Size
    uint8_t latency_timer;   // Latency Timer
    uint8_t header_type;     // Header Type
    uint8_t bist;            // Built-In Self Test (BIST)

    // Offsets 0x10 - 0x27 (Device-specific header)
    uint32_t bar[6];         // Base Address Registers (BAR0 - BAR5)

    // Offsets 0x28 - 0x2B
    uint32_t cardbus_cis;    // CardBus CIS Pointer

    // Offsets 0x2C - 0x2F
    uint16_t subsystem_vendor_id; // Subsystem Vendor ID
    uint16_t subsystem_id;        // Subsystem ID

    // Offsets 0x30 - 0x33
    uint32_t expansion_rom;  // Expansion ROM Base Address

    // Offsets 0x34 - 0x37
    uint8_t capabilities;    // Capabilities Pointer
    uint8_t reserved1[3];    // Reserved

    // Offsets 0x38 - 0x3B
    uint32_t reserved2;      // Reserved

    // Offsets 0x3C - 0x3F
    uint8_t interrupt_line;  // Interrupt Line
    uint8_t interrupt_pin;   // Interrupt Pin
    uint8_t min_grant;       // Minimum Grant
    uint8_t max_latency;     // Maximum Latency

    // weitere Daten, die auch device specific sind

} pci_config_header_t;


// Funktion zum Lesen eines PCI-Registers
uint32_t pci_read_register(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset);

// Funktion zum Schreiben in ein PCI-Register
void pci_write_register(uint8_t bus, uint8_t slot, uint8_t function, uint8_t offset, uint32_t value);

void pci_inspection();