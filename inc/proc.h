
#include "common.h"
#include <stdint.h>

typedef struct {
    uint32_t Reserved1[2];         // Reserved (Offsets 0x000 - 0x008)
    uint32_t ID;                   // APIC ID Register (Offset: 0x020)
    uint32_t Reserved2;
    uint32_t Version;              // APIC Version Register (Offset: 0x030)
    uint32_t Reserved3[4];         // Reserved (Offsets 0x040 - 0x070)
    uint32_t TaskPriority;         // Task Priority Register (Offset: 0x080)
    uint32_t ArbitrationPriority;  // Arbitration Priority Register (Offset: 0x090)
    uint32_t ProcessorPriority;    // Processor Priority Register (Offset: 0x0A0)
    uint32_t EndOfInterrupt;       // End of Interrupt Register (EOI) (Offset: 0x0B0)
    uint32_t RemoteRead;           // Remote Read Register (Offset: 0x0C0)
    uint32_t LogicalDestination;   // Logical Destination Register (Offset: 0x0D0)
    uint32_t DestinationFormat;    // Destination Format Register (Offset: 0x0E0)
    uint32_t SpuriousInterrupt;    // Spurious Interrupt Vector Register (Offset: 0x0F0)
    uint32_t InService[8];         // In-Service Registers (Offsets: 0x100 - 0x170)
    uint32_t TriggerMode[8];       // Trigger Mode Registers (Offsets: 0x180 - 0x1F0)
    uint32_t InterruptRequest[8];  // Interrupt Request Registers (Offsets: 0x200 - 0x270)
    uint32_t ErrorStatus;          // Error Status Register (Offset: 0x280)
    uint32_t Reserved4[6];         // Reserved (Offsets: 0x290 - 0x2F0)
    uint32_t LVT_CMCI;             // Local Vector Table (CMCI) (Offset: 0x2F0)
    uint32_t InterruptCommand[2];  // Interrupt Command Register (ICR) (Offsets: 0x300 - 0x310)
    uint32_t LVT_Timer;            // Local Vector Table (Timer) (Offset: 0x320)
    uint32_t LVT_ThermalSensor;    // Local Vector Table (Thermal Sensor) (Offset: 0x330)
    uint32_t LVT_PerfMon;          // Local Vector Table (Performance Monitoring Counters) (Offset: 0x340)
    uint32_t LVT_LINT0;            // Local Vector Table (LINT0) (Offset: 0x350)
    uint32_t LVT_LINT1;            // Local Vector Table (LINT1) (Offset: 0x360)
    uint32_t LVT_Error;            // Local Vector Table (Error) (Offset: 0x370)
    uint32_t TimerInitialCount;    // Timer Initial Count Register (Offset: 0x380)
    uint32_t TimerCurrentCount;    // Timer Current Count Register (Offset: 0x390)
    uint32_t Reserved5[4];         // Reserved (Offsets: 0x3A0 - 0x3C0)
    uint32_t TimerDivideConfig;    // Timer Divide Configuration Register (Offset: 0x3E0)
    uint32_t Reserved6;            // Reserved (Offset: 0x3F0)
} LOCAL_APIC_STRUCTURE;


#define MADT_TYPE EFI_ACPI_1_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER	// kürzer
#define APIC_TYPE EFI_ACPI_1_0_PROCESSOR_LOCAL_APIC_STRUCTURE		

// Es muss genauer einer dieser Typen aktiviert sein, welcher steuert, wie die Kerne 
// deaktiviert werden sollen

#define SOFT_DISABLE 
//#define HARD_DISABLE

void print_madt(MADT_TYPE *Madt);

EFI_STATUS get_madt(MADT_TYPE **Madt);

EFI_STATUS manipulate_madt(MADT_TYPE *Madt, UINTN noCPUs, APIC_TYPE *apic_entries[]);