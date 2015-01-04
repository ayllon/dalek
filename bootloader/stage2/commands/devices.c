/**
 * devices.c
 */
#include <cli.h>
#include <io.h>
#include <stdio.h>


uint8_t devices(uint8_t argn, const char **argv)
{
    IONode* iterator = io_device_list_begin();
    while (iterator) {
        IODevice* dev = io_device_get_device(iterator);
        printf("%s\t%s\n", dev->name, dev->description);
        iterator = io_device_list_next(iterator);
    }
    return 0;
}

REGISTER_CLI_COMMAND("devices", "Show information about the registered devices", devices);
