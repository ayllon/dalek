/*
 * io.c
 *
 * I/O devices register
 *
 */
#include <io.h>
#include <memory.h>
#include <strings.h>
#include <printf.h>

struct IONode {
    IODevice *device;
    struct IONode *next;
};

static IONode *first_device = NULL;

IODevice *io_register_device(const char *name, const char *description,
        void *data)
{
    IONode *node;
    // New node;
    node = (IONode*) malloc(sizeof(IONode));
    node->next = NULL;
    // New device
    node->device = (IODevice*) malloc(sizeof(IODevice));
    node->device->data = data;
    strlcpy(node->device->name, name, sizeof(node->device->name));
    strlcpy(node->device->description, description, sizeof(node->device->description));
    // Add to list at the beginning
    node->next = first_device;
    first_device = node;
    // Print a message
    log(LOG_INFO, __func__, "New device \"%s\" (%s)", name, description);
    // Return
    return node->device;
}


IONode* io_device_list_begin(void)
{
    return first_device;
}


IONode* io_device_list_next(IONode* node)
{
    if (node)
        return node->next;
    return NULL;
}


IODevice* io_device_get_device(IONode* node)
{
    if (node)
        return node->device;
    return NULL;
}

/**
 * Get the device with the given name
 */
IODevice* io_device_get_by_name(const char* name)
{
    IONode* iterator = io_device_list_begin();
    while (iterator) {
        if (strcmp(iterator->device->name, name) == 0)
            return iterator->device;
    }
    return NULL;
}
