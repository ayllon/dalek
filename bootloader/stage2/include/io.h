/*
 * io.h
 *
 * I/O devices register
 *
 */
#ifndef __IO_H__
#define __IO_H__

#include <types.h>

/**
 * Macro to facilitate compile-time driver registering
 */
typedef void (*io_init_func_ptr)(void);
#define REGISTER_IO(init) \
    static io_init_func_ptr k_io_##init __attribute__((section("__k_io_"#init), used)) = init;

extern io_init_func_ptr __start___k_io[];
extern io_init_func_ptr __stop___k_io[];

/**
 * SEEK whence values
 */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/**
 * IO Device
 */
typedef struct IODevice IODevice;
struct IODevice
{
  // Attributes
  char name[16];
  char description[32];
  void *data;
  // Methods
  off_t   (*seek)(IODevice* self, off_t offset, int whence);
  ssize_t (*read)(IODevice* self, void* buffer, size_t nbytes);
  ssize_t (*write)(IODevice* self, void* buffer, size_t nbytes);
  int     (*ioctl)(IODevice* self, unsigned long request);
};

typedef struct IONode IONode;

/**
 * Initialize the subsystem
 */
void io_init();

/**
 * Register method
 * Registers the new device and return the structure
 * Data should be allocated by the client
 * The custom destroy method will be called when necessary
 */
IODevice *io_register_device(const char *name, const char *description, void *data);

/**
 * Get the first IONode
 */
IONode* io_device_list_begin(void);

/**
 * Get the next IONode entry
 */
IONode* io_device_list_next(IONode* node);

/**
 * Get the IODevice associated with the node
 */
IODevice* io_device_get_device(IONode* node);

/**
 * Get the device with the given name
 */
IODevice* io_device_get_by_name(const char* name);

#endif
