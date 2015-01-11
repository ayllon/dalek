/*
 * memory.h
 */
#ifndef __MM_H__
#define __MM_H__

#include <boot.h>
#include <types.h>
/** DATA STRUCTURES **/

/** Free memory list node */
typedef struct MemoryBlockNode MemoryBlockNode;
struct MemoryBlockNode
{
  void *start;
  MemoryBlockNode *prev, *next;
  size_t size;
};

/** PROTOTYPES **/
void   mm_initialize(const BootInformation* boot_info);
size_t mm_size(void);
void  *malloc(size_t size);
void   free(void *ptr);

void   mm_allocatable_free(size_t *total, size_t *size);
size_t mm_allocatable_used();
const void* mm_allocatable_start();

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *dest, uint8_t val, size_t count);

#endif
