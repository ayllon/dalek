/*
 * chainload.c
 */
#include <cli.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <strings.h>

extern void chainload_asm(int);

uint8_t chainload(uint8_t argc, const char** argv)
{
    const char* dev_name = NULL;
    char force = 0;

    if (argc < 2) {
        log(LOG_ERROR, __func__, "Missing device parameter");
        return  -1;
    }

    dev_name = argv[1];
    if (argc > 2 && strncmp(argv[2], "force", 5) == 0) {
        force = 1;
    }

    IODevice* dev = io_device_get_by_name(dev_name);
    if (!dev) {
        log(LOG_ERROR, __func__, "Could not open %s (%d)", dev_name, errno);
        return -1;
    }

    log(LOG_INFO, __func__, "Chainload from %s", dev_name);
    void* dst = (void*)0x7c00;

    if (io_seek(dev, 0, SEEK_SET) < 0) {
        log(LOG_ERROR, __func__, "Could not reset the position (%d)", errno);
        return -1;
    }
    if (io_read(dev, dst, 512) < 0) {
        log(LOG_ERROR, __func__, "Could not read the first 512 bytes (%d)", errno);
        return -1;
    }

    log(LOG_INFO, __func__, "Loaded 512 bytes into %x", dst);

    // Verify bootable bytes
    unsigned char *sector = (unsigned char*)dst;
    if (!force && sector[510] != 0x55 && sector[511] != 0xAA) {
        log(LOG_ERROR, __func__, "Loaded chunk does not have 0xAA55 marker");
        log(LOG_ERROR, __func__, "Content: 0x%02X%02X", sector[511], sector[510]);
        return -1;
    }

    // De-initialize IO
    io_deinit();

    // Reset screen
    printf("\x1b[37;40m\x1b[J");

    // Leave protected mode and jump
    chainload_asm(dev->id);

    return 0;
}


REGISTER_CLI_COMMAND("chainload", "Chainload from the specified device", chainload);
