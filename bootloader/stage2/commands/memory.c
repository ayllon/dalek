/*
 * memory.c
 *
 */
#include <printf.h>
#include <cli.h>
#include <strings.h>
#include <memory.h>

/**
 * Information about memory
 */
uint8_t memory(uint8_t argn, const char **argv)
{
    if (argn > 1 && strcmp(argv[1], "help")) {
        printf("Shows information about the memory.\nUsage: memory [help]\n");
    }
    else {
        size_t size = mm_size();
        size_t biggest, used;
        extern off_t start, _end;
        /* Memory size */
        printf("Memory size: %z bytes\n\t\t%z KB\n\t\t%z MB\n",
               size, size / 1024, size / (1024 * 1024));
        /* Allocatable */
        mm_allocatable_free(&size, &biggest);
        used = mm_allocatable_used();
        printf("Allocatable memory: %z bytes (%f KB)\n",
               size + used, ((double)(size + used)) / 1024.0);
        printf("\tUsed:\t%z bytes (%f KB)\n",
               used, ((double)used) / 1024.0);
        printf("\tFree:\t%z bytes (%f KB)\n",
                size, ((double)size) / 1024.0);
        printf("\tMax.Contiguous:\t%z bytes\n", biggest);
        /* Stage 2 image */
        printf("Stage 2 start address: 0x%x\n", &start);
        size = ((uint32_t) &_end) - ((uint32_t) &start);
        printf("\tBinary size: %z bytes (%f KB)\n",
               size, ((double)size) / 1024.0);
    }
    return 0;
}
