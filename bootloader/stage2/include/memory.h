/********************************************
 * Memory management
 * Header
 *
 ********************************************/
#ifndef __MM_H__
#define __MM_H__

#define STACK_SIZE 0x1000

#ifndef ASM

#include <types.h>
/** DATA STRUCTURES **/

// Free memory list node
struct memory_block_node
{
  void *start;
  struct memory_block_node *prev, *next;
  uint32 size;
};

/** PROTOTYPES **/
void   mm_initialize(void);
uint32 mm_size(void);
void  *malloc(uint32 size);
void   free(void *ptr);

void   mm_allocatable_free(uint32 *total, uint32 *size);
uint32 mm_allocatable_used();

uint8  *memcpy(uint8 *dest, uint8 *src, uint16 count);
uint8  *memset(uint8 *dest, uint8 val, uint16 count);
uint16 *memsetw(uint8 *dest, uint16 val, uint16 count);

#endif
#endif
