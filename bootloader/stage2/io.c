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

typedef struct STIONode {
    IODevice *device;
    struct STIONode *next;
} IONode;

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
    log(__func__, "New device \"%s\" (%s)", name, description);
    // Return
    return node->device;
}
