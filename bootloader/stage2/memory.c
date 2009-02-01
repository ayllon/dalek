/********************************************
 * Memory management
 *
 ********************************************/
#include <memory.h>
#include <types.h>
#include <printf.h>

// Memory information
static uint32 mem_size = 0; // In KB!!

static struct memory_block_node *mem_free_blocks_list;
static struct memory_block_node *mem_used_blocks_list;

/** void mm_get_memory_size()
 * Test the memory to get the size
 */
static void mm_get_memory_size(void)
{
  // todo: usar bios
}

/** UINT mm_size()
 * Return the memory size in bytes
 */
uint32 mm_size(void)
{
  return mem_size;
}

/** void mm_initialize()
 * Initializes the memory manager
 */
void mm_initialize(void)
{
  extern unsigned long _end;
  struct memory_block_node *mem_node;

  // Get memory size
  mm_get_memory_size();

  // Used memory: kernel space, plus first node of each list here, plus stack
  mem_used_blocks_list = (void*)(0x100000 + _end + STACK_SIZE);

  mem_node = mem_used_blocks_list;

  mem_node->start = 0x00;
  mem_node->size = (uint32)(mem_node + 2*sizeof(struct memory_block_node));
  mem_node->prev = mem_node->next = NULL;

  // Free memory: the rest
  mem_free_blocks_list = mem_used_blocks_list + sizeof(struct memory_block_node);

  mem_node = mem_free_blocks_list;

  mem_node->start = mem_node + sizeof(struct memory_block_node);
  mem_node->size = mem_size - (uint32)(mem_node->start);
  mem_node->prev = mem_node->next = NULL;

}

/** void *malloc(uint32 size)
 * Returns a pointer to a free memory area and
 * marks it as used
 */
void *malloc(uint32 size)
{
  struct memory_block_node *mem_node, *used_node;
  uint32 block_size = size + sizeof(struct memory_block_node);

  mem_node = mem_free_blocks_list;

  // Search for a free block
  while(mem_node && mem_node->size < block_size)
    mem_node = mem_node->next;

  // Not enought
  if(!mem_node)
  {
    printf("[kmalloc()] Not enough free memory (%i bytes)", size);
    return NULL;
  }

  // We have a block that it is enought
  // Mark as used (we've got space for this node also)
  // Insert the node at the beggining (it is fastest)
  used_node            = mem_node->start;

  used_node->size      = block_size;
  used_node->start     = used_node;
  used_node->prev      = NULL;
  used_node->next      = mem_used_blocks_list;
  if(used_node->next)
    used_node->next->prev = used_node;
  mem_used_blocks_list = used_node;

  // Reduce the free node
  mem_node->size  -= block_size;
  mem_node->start += block_size;

  // Have we used all the block?
  if(mem_node->size < 0)
  {
    if(mem_node->prev)
      mem_node->prev = mem_node->next;
    else
      mem_free_blocks_list = mem_node->next;

    if(mem_node->next)
      mem_node->next->prev = mem_node->prev;

    // Node deleted. Free the memory
    free(mem_node);
  }

  // Return
  return used_node->start + sizeof(struct memory_block_node);
}

/** free()
 * Frees a memory block
 */
void free(void *ptr)
{
  struct memory_block_node *mem_node, *free_node;
  void *offset;

  // Search the block (be careful: the pointer points to start+sizeof(...)
  mem_node = mem_used_blocks_list;
  offset = ptr - sizeof(struct memory_block_node);

  while(mem_node)
  {
    if(mem_node->start == offset)
      break;
    mem_node = mem_node->next;
  }

  // Do we have the node?
  if(!mem_node)
    printf("[free()] Not reserved address (with malloc)\n");
  else
  {
    // Delete node from used list
    if(mem_node->prev)
      mem_node->prev->next = mem_node->next;
    else
      mem_used_blocks_list = mem_node->next;

    if(mem_node->next)
      mem_node->next->prev = mem_node->prev;

    // Add to free list
    // We iterate through the list, if we found an adjacent block,
    // merge
    free_node = mem_free_blocks_list;
    while(free_node)
    {
      // Before
      if(free_node->start + free_node->size == mem_node->start)
      {
	free_node->size += mem_node->size;
	return;
      }
      // After
      else if(mem_node->start + mem_node->size == free_node->start)
      {
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

/* memset()
 */
uint8 *memset(uint8 *dest, uint8 val, uint16 count)
{
  uint8 *end = dest + count;

  while(dest != end)
  {
    *dest = val;
    dest++;
  }

  return dest;
}

/* memcpy()
 */
uint8 *memcpy(uint8 *dest, uint8 *src, uint16 count)
{
  unsigned char *end = dest + count;

  while(dest != end)
  {
    *dest = *src;
    dest++;
    src++;
  }

  return dest;
}
