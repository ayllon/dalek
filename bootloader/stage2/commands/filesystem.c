/*
 * filesystem.c
 *
 * Filesystem related commands
 */

#include <cli.h>
#include <fs.h>
#include <io.h>
#include <stdio.h>


/**
 * Selects a device as root
 */
uint8_t root(uint8_t argn, const char **argv)
{
    if (argn != 2) {
        log(LOG_ERROR, __func__, "Need the device to set up as root");
        return -1;
    }
    const char* dev_name = argv[1];

    IODevice* dev = io_device_get_by_name(dev_name);
    if (!dev) {
        log(LOG_ERROR, __func__, "Could not find the device %s", dev_name);
        return -1;
    }

    if (fs_set_root(dev) < 0) {
        log(LOG_ERROR, __func__, "Could not mount %s", dev_name);
        return -1;
    }

    log(LOG_INFO, __func__, "%s mounted as root", dev_name);
    return 0;
}


REGISTER_CLI_COMMAND("root", "Select a device as root", root);
