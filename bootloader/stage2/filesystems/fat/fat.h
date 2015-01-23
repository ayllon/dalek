/*
 * fat.h
 */

#ifndef __FAT_H__
#define __FAT_H__

#include <types.h>

// 0x28 is also a valid signature with a different EBPB layout,
// but we just ignore it
#define FAT_SIGNATURE 0x29

typedef struct Fat_1x_EBPB {
    uint8_t  drive_number; // Drive number
    uint8_t  reserved; // Reserved
    uint8_t  signature; // Signature
    uint32_t serial; // Serial number
    uint8_t  label[11]; // Volume label
    uint8_t  system_id[8]; // System identifier string
    uint8_t boot_code[448]; // Boot code
} __attribute__((packed)) Fat_1x_EBPB;

typedef struct Fat_32_EBPB {
    uint32_t sec_per_fat; // Sectors per FAT
    uint16_t flags; // Flags
    uint16_t version; // Version number. High is major, low is minor
    uint32_t root_cluster; // The cluster number of the root directory
    uint16_t fs_info_sector; // The sector number of the FSInfo structure
    uint16_t boot_backup_sector; //  The sector number of the backup boot sector
    uint8_t  reserved[8]; // Reserved
    uint8_t  drive_number; // Drive number
    uint8_t  winnt_flags; // Flags for WinNT, reserved otherwise
    uint8_t  signature; // Signature (must be 0x28 or 0x29).
    uint32_t serial; // Serial number
    uint8_t  label[11]; // Volume label
    uint8_t system_id[8]; // System identifier string
    uint8_t boot_code[420]; // Boot code
} __attribute__((packed)) Fat_32_EBPB;

typedef struct FatBS {
    uint8_t  jmp[3]; // jmp short XX nop
    uint8_t  oem[8]; // OEM identifier
    uint16_t bytes_per_sec; // Bytes per sector
    uint8_t  sec_per_cluster; // Sectors per cluster
    uint16_t n_reserved_sec; // Number of reserved sectors
    uint8_t  n_fat; // Number of FAT
    uint16_t root_max_entries; // Max number of directory entries
    uint16_t n_sectors;
    uint8_t  media;
    uint16_t sec_per_fat; // Sectors per FAT
    uint16_t sec_per_track; // Sectors per track
    int16_t  n_heads; // Number of heads
    uint32_t hidden_sectors; // Number of hidden sectors
    uint32_t large_amount_sectors; // If set, there are more than 65535 sectors in the volume

    union {
        Fat_1x_EBPB fat1x;
        Fat_32_EBPB fat32;
    };

    uint8_t bootable[2]; // Bootable signature (0xAA55)
} __attribute__((packed)) FatBS;


#endif /* __FAT_H__ */
