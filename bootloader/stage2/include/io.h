/*
 * io.h
 *
 * I/O devices register
 *
 */
#ifndef __IO_H__
#define __IO_H__

typedef struct
{
  // Attributes
  char name[16];
  char description[32];
  void *data;
  // Methods
  // read, write, seek, free, etc...
}IODevice;

/* Register method
 * Registers the new device and return the structure
 * Data should be allocated by the client
 * The custom destroy method will be called when necessary
 */
IODevice *io_register_device(const char *name, const char *description, void *data);

#endif
