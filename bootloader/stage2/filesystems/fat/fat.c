/*
 * fat.c
 */
#include <errno.h>
#include <fs.h>
#include <io.h>
#include <memory.h>
#include <modules.h>
#include <stdio.h>
#include "fat.h"

/* FAT specific FSHandle */
struct FSHandle {
    FatBS fat_bs;
    enum {FAT12 = 0, FAT16 = 1, FAT32 = 2} fat_type;
    uint32_t root_dir_sec;
    uint32_t data_sec;
    uint32_t first_data_sec;
    uint32_t n_clusters;
    uint32_t fat_size;
    uint8_t* fat;
};

static const char *FAT_TYPE_STR[] = {
    "FAT12", "FAT16", "FAT32"
};


FSHandle* fat_get_handle(IODevice* dev)
{
    FSHandle fh;

    if (io_seek(dev, 0, SEEK_SET) < 0 ||
        io_read(dev, &fh.fat_bs, sizeof(FatBS)) < 0) {
        log(LOG_ERROR, __func__, "Could not read initial sector (%d)", errno);
        return NULL;
    }

    if (fh.fat_bs.bytes_per_sec == 0) {
        log(LOG_DEBUG, __func__, "Bytes per sec is 0, skip");
        return NULL;
    }
    if (fh.fat_bs.sec_per_cluster == 0) {
        log(LOG_DEBUG, __func__, "Sectors per cluster is 0, skip");
        return NULL;
    }

    fh.fat_size = fh.fat_bs.sec_per_fat * fh.fat_bs.bytes_per_sec;

    fh.root_dir_sec = (
            (fh.fat_bs.root_max_entries * 32)
            + (fh.fat_bs.bytes_per_sec - 1)
        ) / fh.fat_bs.bytes_per_sec;

    fh.first_data_sec = fh.fat_bs.n_reserved_sec + (fh.fat_bs.n_fat * fh.fat_bs.sec_per_fat);

    fh.data_sec = fh.fat_bs.n_sectors - (
            fh.fat_bs.n_reserved_sec + (fh.fat_bs.n_fat * fh.fat_bs.sec_per_fat)
            + fh.root_dir_sec
        );

    fh.n_clusters = fh.data_sec / fh.fat_bs.sec_per_cluster;

    // Guess type
    if (fh.n_clusters < 4085)
        fh.fat_type = FAT12;
    else if (fh.n_clusters < 65525)
        fh.fat_type = FAT16;
    else
        fh.fat_type = FAT32;

    // All this was assuming it was indeed FAT
    // Examine the signature depending on the type that is should be
    switch (fh.fat_type) {
        case FAT12: case FAT16:
            if (fh.fat_bs.fat1x.signature != FAT_SIGNATURE) {
                log(LOG_DEBUG, __func__, "FAT1x signature does not match (0x%x)", fh.fat_bs.fat1x.signature);
                return NULL;
            }
            break;
        default:
            if (fh.fat_bs.fat32.signature != FAT_SIGNATURE) {
                log(LOG_DEBUG, __func__, "FAT32 signature does not match (0x%x)", fh.fat_bs.fat32.signature);
                return NULL;
            }
    }

    // We got something!
    log(LOG_INFO, __func__, "Identified a %s filesystem on device %s",
            FAT_TYPE_STR[fh.fat_type], dev->name);
    if (fh.fat_type == FAT32)
        log(LOG_INFO, __func__, "[OEM %.8s] %.11s", fh.fat_bs.oem, fh.fat_bs.fat32.label);
    else
        log(LOG_INFO, __func__, "[OEM %.8s] %.11s", fh.fat_bs.oem, fh.fat_bs.fat1x.label);

    // Only FAT12 for the moment
    if (fh.fat_type == FAT32) {
        log(LOG_ERROR, __func__, "Only FATx for now");
        return NULL;
    }

    // Allocate space for the table and read
    fh.fat = malloc(fh.fat_size);
    if (io_seek(dev, fh.fat_bs.n_reserved_sec * 512, SEEK_SET) < 0 ||
        io_read(dev, fh.fat, fh.fat_size) < 0) {
        free(fh.fat);
        log(LOG_ERROR, __func__, "Failed to read the FAT (%d)", errno);
        return NULL;
    }

    // Read root
    FatDirEntry entries[fh.fat_bs.root_max_entries];
    io_seek(dev, fh.first_data_sec * 512, SEEK_SET);
    io_read(dev, &entries, fh.fat_bs.root_max_entries * sizeof(FatDirEntry));
    int i;
    for (i = 0; entries[i].name[0]; ++i) {
        if ((entries[i].attr ^ FAT_VOLUME_LABEL) && entries[i].attr != 0x0F)
            printf("%.8s.%.3s\n", entries[i].name, entries[i].ext);
    }

    // Return a pointer to the heap
    FSHandle* handle = malloc(sizeof(*handle));
    memcpy(handle, &fh, sizeof(*handle));
    return handle;
}


void fat_release_handle(FSHandle* handle)
{
    free(handle->fat);
    free(handle);
}


ino_t fat_get_root(FSHandle* handle)
{
    // Always for FAT1x
    return 0x00;
}


static FileSystemImpl fat_fs = {
    .next = NULL,
    .name = "FAT",
    .get_handle = fat_get_handle,
    .release_handle = fat_release_handle,
    .get_root = fat_get_root,
    .opendir = NULL,
    .readdir = NULL,
    .closedir = NULL,
};


static int fat_init(void)
{
    fs_register(&fat_fs);
    return 0;
}


MODULE_INIT(fat_init);
