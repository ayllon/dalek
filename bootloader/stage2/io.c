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

typedef struct STIONode
{
  IODevice *device;
  struct STIONode *next;
}IONode;

static IONode *first_device = NULL;

IODevice *io_register_device(const char *name, const char *description, void *data)
{
  IONode *node;
  // New node;
  node = (IONode*)malloc(sizeof(IONode));
  node->next = NULL;
  // New device
  node->device = (IODevice*)malloc(sizeof(IODevice));
  node->device->data = data;
  strcpy(node->device->name, name);
  strcpy(node->device->description, description);
  // Add to list at the beggining
  node->next = first_device;
  first_device = node;
  // Print a message
  setcolor(LIGHT_RED, BLUE);
  printf("[io_register_device()] New device \"%s\" (%s)\n", name, description);
  restorecolor();
  // Return
  return node->device;
}
