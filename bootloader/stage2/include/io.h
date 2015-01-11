/*
 * io.h
 *
 * I/O devices register
 *
 */
#ifndef __IO_H__
#define __IO_H__

#include <stdarg.h>
#include <types.h>

/**
 * Macro to facilitate compile-time driver registering
 */
typedef struct IOImpl {
    void (*init)(void);
    void (*deinit)(void);
} IOImpl;

#define REGISTER_IO_LVL(init, deinit, level) \
    static IOImpl k_io_##init __attribute__((section("__k_io_"#level), used)) = { init, deinit };

#define REGISTER_IO(init, deinit) \
    REGISTER_IO_LVL(init, deinit, 99)
#define REGISTER_IO_EARLY(init, deinit) \
	REGISTER_IO_LVL(init, deinit, 0)

extern IOImpl __start___k_io[];
extern IOImpl __stop___k_io[];

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
  int   id;
  // Methods
  off_t   (*seek)(IODevice* self, off_t offset, int whence);
  ssize_t (*read)(IODevice* self, void* buffer, size_t nbytes);
  ssize_t (*write)(IODevice* self, const void* buffer, size_t nbytes);
  int     (*ioctl)(IODevice* self, const char* request, va_list args);
};

typedef struct IONode IONode;

/**
 * Initialize the subsystem
 */
void io_init();

/**
 * Deinitialize the subsystem
 */
void io_deinit();

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

/**
 * Seek
 */
off_t io_seek(IODevice* dev, off_t offset, int whence);

/**
 * Read
 */
ssize_t io_read(IODevice* dev, void* buffer, size_t nbytes);

/**
 * Write
 */
ssize_t io_write(IODevice* dev, const void* buffer, size_t nbytes);

/**
 * IOCtl
 */
int io_ioctl(IODevice* dev, const char* request, ...);

#endif
