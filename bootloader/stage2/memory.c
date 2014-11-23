/*
 * Memory management
 */
#include <memory.h>
#include <types.h>
#include <printf.h>
#include <ports.h>

// Memory information
static uint32 mem_size = 0; // In KB!!

static struct memory_block_node *mem_free_blocks_list;
static struct memory_block_node *mem_used_blocks_list;

/**
 * Test the memory to get the size
 */
static void mm_get_memory_size(void)
{
  /*  register uint32 *mem, a, cr0;
  uint32 mem_count;

  // Copy of CR0
  asm volatile ("movl %%cr0, %%eax" : "=a"(cr0));

  // Flush the cache and invalidate
  asm volatile ("wbinvd");

  // Disable cache, writeback, and use 32 bit mode
  asm volatile("movl %%eax, %%cr0" : : "a"(cr0 | 0x00000001 | 0x40000000 | 0x20000000));

  // Remember the 640 KB barrier. Start at 1 MB
  mem_count = 1024 * 1024;
  mem_size  = 1024;

  do{
    mem_size += 1024;
    printf("Memory testing %i KB ...\r", mem_size);
    mem_count += 1024 * 1024;
    mem = (uint32*)mem_count;

    a = *mem;

    *mem = 0x55AA55AA;
    // The empty ASM tells the compiler that it musn't
    // rely on its registers as saved variables
    asm("":::"memory");
    if(*mem != 0x55AA55AA)
      mem_count = 0;
    else
    {
      *mem = 0xAA55AA55;
      asm("":::"memory");
      if(*mem != 0xAA55AA55)
	mem_count = 0;
    }
    asm("":::"memory");
    *mem = a;
  }while(mem_size < 4190208 && mem_count != 0);

  printf("\n");

  // Restore CR0
  asm volatile("movl %%eax, %%cr0": : "a"(cr0));

  // mem_size in KB, we want bytes
  mem_size *= 1024;*/

  uint8 lowmem, himem;

  // Ask CMOS
  outportb(0x70, 0x30);
  lowmem = inportb(0x71);
  outportb(0x70, 0x31);
  himem = inportb(0x71);

  // Concatenate
  mem_size = ((himem * 0xFF) + lowmem) * 1024;
}

/**
 * Return the memory size in bytes
 */
uint32 mm_size(void)
{
  return mem_size;
}

/**
 * Initializes the memory manager
 * Only the memory between the loader image and 1MB barrier can be used
 * (Kernel image is loaded from 1MB)
 */
void mm_initialize(void)
{
    struct memory_block_node *mem_node;
    extern uint32 _end;

    // Get memory size
    mm_get_memory_size();

    // Start of free memory
    mem_used_blocks_list = (struct memory_block_node*) &_end;

    mem_node = mem_used_blocks_list;

    mem_node->start = 0x00;
    mem_node->size = (uint32) (mem_node + 2 * sizeof(struct memory_block_node));
    mem_node->prev = mem_node->next = NULL;

    // Free memory: the rest until 1MB barrier
    mem_free_blocks_list = mem_used_blocks_list
            + sizeof(struct memory_block_node);

    mem_node = mem_free_blocks_list;

    mem_node->start = mem_node + sizeof(struct memory_block_node);
    mem_node->size = 0x100000 - (uint32) (mem_node->start);
    mem_node->prev = mem_node->next = NULL;

}

/**
 * Returns a pointer to a free memory area and
 * marks it as used
 */
void *malloc(uint32 size)
{
    struct memory_block_node *mem_node, *used_node;
    uint32 block_size = size + sizeof(struct memory_block_node);
    uint32 biggest = 0;

    mem_node = mem_free_blocks_list;

    // Search for a free block
    while (mem_node && mem_node->size < block_size) {
        if (mem_node->size > biggest)
            biggest = mem_node->size;
        mem_node = mem_node->next;
    }

    // Not enought
    if (!mem_node) {
        printf(
                "[malloc()] Not enough free memory (%i bytes needed / %i biggest block)\n",
                size);
        return NULL ;
    }

    // We have a block that it is enought
    // Mark as used (we've got space for this node also)
    // Insert the node at the beggining (it is fastest)
    used_node = mem_node->start;

    used_node->size = block_size;
    used_node->start = used_node;
    used_node->prev = NULL;
    used_node->next = mem_used_blocks_list;
    if (used_node->next)
        used_node->next->prev = used_node;
    mem_used_blocks_list = used_node;

    // Reduce the free node
    mem_node->size -= block_size;
    mem_node->start += block_size;

    // Have we used all the block?
    if (mem_node->size < 0) {
        if (mem_node->prev)
            mem_node->prev = mem_node->next;
        else
            mem_free_blocks_list = mem_node->next;

        if (mem_node->next)
            mem_node->next->prev = mem_node->prev;

        // Node deleted. Free the memory
        free(mem_node);
    }

    // Return
    return used_node->start + sizeof(struct memory_block_node);
}

/**
 * Frees a memory block
 */
void free(void *ptr)
{
    struct memory_block_node *mem_node, *free_node;
    void *offset;

    // Search the block (be careful: the pointer points to start+sizeof(...)
    mem_node = mem_used_blocks_list;
    offset = ptr - sizeof(struct memory_block_node);

    while (mem_node) {
        if (mem_node->start == offset)
            break;
        mem_node = mem_node->next;
    }

    // Do we have the node?
    if (!mem_node)
        printf("[free()] Not reserved address (with malloc)\n");
    else {
        // Delete node from used list
        if (mem_node->prev)
            mem_node->prev->next = mem_node->next;
        else
            mem_used_blocks_list = mem_node->next;

        if (mem_node->next)
            mem_node->next->prev = mem_node->prev;

        // Add to free list
        // We iterate through the list, if we found an adjacent block,
        // merge
        free_node = mem_free_blocks_list;
        while (free_node) {
            // Before
            if (free_node->start + free_node->size == mem_node->start) {
                free_node->size += mem_node->size;
                return;
            }
            // After
            else if (mem_node->start + mem_node->size == free_node->start) {
                free_node->size += mem_node->size;
                free_node->start = mem_node->start;
                return;
            }

            // Next block
            free_node = free_node->next;
        }
        // Otherwise, new node
        free_node = malloc(sizeof(struct memory_block_node));
        free_node->start = mem_node->start;
        free_node->size = mem_node->size;
        free_node->prev = NULL;
        free_node->next = mem_free_blocks_list;
        mem_free_blocks_list = free_node;
    }
}

/**
 * Set to val count bytes pointed by dest
 */
uint8 *memset(uint8 *dest, uint8 val, uint16 count)
{
    uint8 *end = dest + count;

    while (dest != end) {
        *dest = val;
        dest++;
    }

    return dest;
}

/**
 * Copy memory from src to dest
 */
uint8 *memcpy(uint8 *dest, uint8 *src, uint16 count)
{
    unsigned char *end = dest + count;

    while (dest != end) {
        *dest = *src;
        dest++;
        src++;
    }

    return dest;
}

/**
 * Tells how much memory we have for dynamic allocation (free)
 * Puts into "total" the total amount, and in "biggest" the biggest free block
 */
void mm_allocatable_free(uint32 *total, uint32 *biggest)
{
    struct memory_block_node *mem_node;

    // Init
    *biggest = 0;
    *total = 0;
    // Compute
    for (mem_node = mem_free_blocks_list; mem_node; mem_node = mem_node->next) {
        *total += mem_node->size;
        if (mem_node->size > *biggest)
            *biggest = mem_node->size;
    }
}

/**
 * Tells how many of the allocatable memory is used
 */
uint32 mm_allocatable_used()
{
    struct memory_block_node *mem_node;
    uint32 s;

    s = 0;

    for (mem_node = mem_used_blocks_list; mem_node; mem_node = mem_node->next)
        s += mem_node->size;

    return s;
}
