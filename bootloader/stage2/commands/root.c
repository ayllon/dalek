/*
 * root.c
 */
#include <cli.h>
#include <fs.h>
#include <io.h>
#include <stdio.h>


uint8_t root(uint8_t argc, char** argv)
{
    if (argc < 2) {
        log(LOG_ERROR, __func__, "Missing device parameter");
        return -1;
    }
    IODevice* dev = io_device_get_by_name(argv[1]);
    if (fs_set_root(dev) < 0) {
        log(LOG_ERROR, __func__, "Failed to set a root device (%d)", errno);
        return -1;
    }
    log(LOG_INFO, __func__, "%s set as root device", argv[1]);
    return 0;
}
