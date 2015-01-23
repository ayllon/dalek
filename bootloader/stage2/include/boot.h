/*
 * boot.h
 */

#ifndef __BOOT_H__
#define __BOOT_H__

#include <types.h>

#define SMAP_ENTRY_TYPE_USABLE           1
#define SMAP_ENTRY_TYPE_RESERVED         2
#define SMAP_ENTRY_TYPE_ACPI_RECLAIMABLE 3
#define SMAP_ENTRY_TYPE_ACPI_NVS         4
#define SMAP_ENTRY_TYPE_BAD              5

#define SMAP_ENTRY_IS_USABLE(ent) \
    ((ent->acpi & 0x01) && (ent->type == SMAP_ENTRY_TYPE_USABLE))

/* Data passed by assembler code to main */
typedef struct
{
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) SMAPEntry;

typedef struct
{
    uint8_t boot_drive;
    uint16_t smap_size;
    SMAPEntry smap_entries[];
} __attribute__((packed)) BootInformation;

#endif
