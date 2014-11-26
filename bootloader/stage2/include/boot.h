/*
 * boot.h
 */

#ifndef __BOOT_H__
#define __BOOT_H__

#include <types.h>

/* Data passed by assembler code to main */
typedef struct
{
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) SMAPEntry;

typedef struct
{
    uint8_t boot_drive;
    uint8_t smap_size;
    SMAPEntry* smap_entries;
} BootInformation;

#endif
