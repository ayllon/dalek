/*
 * read.c
 */
#include <cli.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>
#include <strings.h>


uint8_t read(uint8_t argn, const char** argv)
{
    size_t nbytes = 512;
    const char* dev_name = NULL;
    if (argn > 1)
        dev_name = argv[1];
    if (argn > 2)
        nbytes = atoi(argv[2]);

    if (!dev_name) {
        log(LOG_ERROR, __func__, "Missing device name");
        return -1;
    }

    IODevice* dev = io_device_get_by_name(dev_name);
    if (!dev) {
        log(LOG_ERROR, __func__, "Could not find %s (%d)", dev_name, errno);
        return -1;
    }
    if (!dev->read) {
        log(LOG_ERROR, __func__, "%s does not implement read", dev_name);
        return -1;
    }

    uint8_t buffer[nbytes];
    if (dev->read(dev, buffer, nbytes) < 0) {
        log(LOG_ERROR, __func__, "Failed to read: %d", errno);
        return -1;
    }

    /* Format nicely */
    int line = 0, nlines = nbytes / 16;
    int i = 0, w = 0;

    if (nbytes % 16 > 0)
        nlines++;

    for (line = 0; line < nlines; ++line) {
        printf("%08x\t", line * 16);
        w = 0;
        while (i < nbytes && w < 16) {
            printf("%02x ", (unsigned)buffer[i]);
            ++i;
            ++w;
        }
        printf("\n");
    }

    return 0;
}

uint8_t seek(uint8_t argn, const char** argv)
{
    off_t offset = 0;
    const char* dev_name = NULL;
    if (argn > 1)
        dev_name = argv[1];
    if (argn > 2)
        offset = atoi(argv[2]);

    if (!dev_name) {
        log(LOG_ERROR, __func__, "Missing device name");
        return -1;
    }

    IODevice* dev = io_device_get_by_name(dev_name);
    if (!dev) {
        log(LOG_ERROR, __func__, "Could not find %s (%d)", dev_name, errno);
        return -1;
    }
    if (!dev->seek) {
        log(LOG_ERROR, __func__, "%s does not implement seek", dev_name);
        return -1;
    }

    if (dev->seek(dev, offset, SEEK_SET) < 0) {
        log(LOG_ERROR, __func__, "%s failed to seek to %d (%d)", dev_name, offset, errno);
        return -1;
    }

    return 0;
}

REGISTER_CLI_COMMAND("read", "read dev [nbytes] (max 512 bytes)", read);
REGISTER_CLI_COMMAND("seek", "seek dev [absolute]", seek);
