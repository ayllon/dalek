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
        extern uint32_t start, _end;
        /* Memory size */
        printf("Memory size: %i bytes\n\t\t%i KB\n\t\t%i MB\n", size,
                (size / 1024), (size / (1024 * 1024)));
        /* Allocatable */
        mm_allocatable_free(&size, &biggest);
        used = mm_allocatable_used();
        printf("Allocatable memory: %i bytes (%f KB)\n", size + used,
                (size + used) / 1024.0);
        printf("\tUsed:\t%i bytes (%f KB)\n", used, used / 1024.0);
        printf("\tFree:\t%i bytes (%f KB)\n", size, size / 1024.0);
        printf("\tMax.Contiguous:\t%i bytes\n", biggest);
        /* Stage 2 image */
        printf("Stage 2 start address: 0x%x\n", &start);
        size = ((uint32_t) &_end) - ((uint32_t) &start);
        printf("\tBinary size: %i bytes (%f KB)\n", size, size / 1024.0);
    }
    return 0;
}
