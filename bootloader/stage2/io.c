/*
 * io.c
 *
 * I/O devices register
 *
 */
#include <errno.h>
#include <io.h>
#include <memory.h>
#include <stdio.h>
#include <strings.h>

struct IONode {
    IODevice *device;
    struct IONode *next;
};

static IONode *first_device;


void io_init()
{
    first_device = NULL;
    io_init_func_ptr* io_init;

    log(LOG_INFO, __func__, "Initializing devices");

    for (io_init = __start___k_io; io_init != __stop___k_io; ++io_init) {
        (*io_init)();
    }

    log(LOG_INFO, __func__, "Devices initialized");
}


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


IODevice* io_device_get_by_name(const char* name)
{
    IONode* iterator = io_device_list_begin();
    while (iterator) {
        if (strcmp(iterator->device->name, name) == 0)
            return iterator->device;
        iterator = io_device_list_next(iterator);
    }
    errno = ENOENT;
    return NULL;
}


off_t io_seek(IODevice* dev, off_t offset, int whence)
{
    if (!dev) {
        errno = EFAULT;
        return -1;
    }
    if (!dev->seek) {
        errno = ENOSYS;
        return -1;
    }
    return dev->seek(dev, offset, whence);
}


ssize_t io_read(IODevice* dev, void* buffer, size_t nbytes)
{
    if (!dev) {
        errno = EFAULT;
        return -1;
    }
    if (!dev->read) {
        errno = ENOSYS;
        return -1;
    }
    return dev->read(dev, buffer, nbytes);
}


ssize_t io_write(IODevice* dev, const void* buffer, size_t nbytes)
{
    if (!dev) {
        errno = EFAULT;
        return -1;
    }
    if (!dev->write) {
        errno = ENOSYS;
        return -1;
    }
    return dev->write(dev, buffer, nbytes);
}


int io_ioctl(IODevice* dev, const char* request, ...)
{
    if (!dev) {
        errno = EFAULT;
        return -1;
    }
    if (!dev->ioctl) {
        errno = ENOSYS;
        return -1;
    }
    va_list args;
    va_start(args, request);
    int r = dev->ioctl(dev, request, args);
    va_end(args);
    return r;
}
