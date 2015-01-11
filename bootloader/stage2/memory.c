/*
 * Memory management
 */
#include <memory.h>
#include <types.h>
#include <ports.h>
#include <stdio.h>
#include <panic.h>

// Memory information
static uint64_t mem_size = 0; // In KB!!

const SMAPEntry *mem_pool = NULL;;
static MemoryBlockNode *mem_free_blocks_list;
static MemoryBlockNode *mem_used_blocks_list;

/**
 * Test the memory to get the size
 */
static void mm_get_memory_size(const BootInformation* boot_info)
{
    uint16_t i;
    for (i = 0; i < boot_info->smap_size; ++i) {
        const SMAPEntry *ent = &(boot_info->smap_entries[i]);
        if (SMAP_ENTRY_IS_USABLE(ent)) {
            mem_size += ent->length;
        }
    }
}

/**
 * Return the memory size in bytes
 */
size_t mm_size(void)
{
  return mem_size;
}

/**
 * Initializes the memory manager
 */
void mm_initialize(const BootInformation* boot_info)
{
    MemoryBlockNode *mem_node;

    // Get memory size
    mm_get_memory_size(boot_info);

    // Use the first block over 1MB as memory pool
    uint16_t i;
    for (i = 0; i < boot_info->smap_size; ++i) {
        const SMAPEntry *ent = &(boot_info->smap_entries[i]);
        if (SMAP_ENTRY_IS_USABLE(ent) && ent->base >= 0x100000) {
            mem_pool = ent;
        }
    }

    if (!mem_pool) {
        panic(__func__, "Could not find any available memory pool");
    }

    // Start of free memory
    mem_used_blocks_list = (MemoryBlockNode*) mem_pool->base;

    mem_node = mem_used_blocks_list;

    mem_node->start = (void*)mem_pool->base;
    mem_node->size = sizeof(MemoryBlockNode);
    mem_node->prev = mem_node->next = NULL;

    // Free memory: the rest
    mem_free_blocks_list = mem_used_blocks_list
            + sizeof(MemoryBlockNode);

    mem_node = mem_free_blocks_list;

    mem_node->start = mem_node + sizeof(MemoryBlockNode);
    mem_node->size = mem_pool->length - (uint32_t) (mem_node->start);
    mem_node->prev = mem_node->next = NULL;

}

/**
 * Returns a pointer to a free memory area and
 * marks it as used
 */
void *malloc(size_t size)
{
    MemoryBlockNode *mem_node, *used_node;
    uint32_t block_size = size + sizeof(MemoryBlockNode);
    uint32_t biggest = 0;

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

    // We have a block that it is enough
    // Mark as used (we've got space for this node also)
    // Insert the node at the beginning (it is fastest)
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
    return used_node->start + sizeof(MemoryBlockNode);
}

/**
 * Frees a memory block
 */
void free(void *ptr)
{
    MemoryBlockNode *mem_node, *free_node;
    void *offset;

    // Search the block (be careful: the pointer points to start+sizeof(...)
    mem_node = mem_used_blocks_list;
    offset = ptr - sizeof(MemoryBlockNode);

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
        free_node = malloc(sizeof(MemoryBlockNode));
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
void *memset(void *dest, uint8_t val, size_t count)
{
    uint8_t *end = dest + count;
    uint8_t* p = dest;

    while (p != end) {
        *p = val;
        p++;
    }

    return dest;
}

/**
 * Copy memory from src to dest
 */
void *memcpy(void *dest, const void *src, size_t count)
{
    uint8_t *end = dest + count;
    const uint8_t * ps = (const uint8_t*)src;
    uint8_t *pd = (uint8_t*)dest;

    while (pd != end) {
        *pd = *ps;
        ps++;
        pd++;
    }

    return dest;
}

/**
 * Tells how much memory we have for dynamic allocation (free)
 * Puts into "total" the total amount, and in "biggest" the biggest free block
 */
void mm_allocatable_free(size_t *total, size_t *biggest)
{
    MemoryBlockNode *mem_node;

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
size_t mm_allocatable_used()
{
    MemoryBlockNode *mem_node;
    uint32_t s;

    s = 0;

    for (mem_node = mem_used_blocks_list; mem_node; mem_node = mem_node->next)
        s += mem_node->size;

    return s;
}

/**
 * Start position of the allocatable pool
 */
const void* mm_allocatable_start()
{
    return (const void*)mem_pool->base;
}
