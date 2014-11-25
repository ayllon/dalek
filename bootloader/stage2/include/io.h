/*
 * io.h
 *
 * I/O devices register
 *
 */
#ifndef __IO_H__
#define __IO_H__

#include <types.h>

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

/**
 * Register method
 * Registers the new device and return the structure
 * Data should be allocated by the client
 * The custom destroy method will be called when necessary
 */
IODevice *io_register_device(const char *name, const char *description, void *data);

#endif
