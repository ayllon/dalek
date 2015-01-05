/* Main file */
#include <boot.h>
#include <cli.h>
#include <idt.h>
#include <io.h>
#include <irq.h>
#include <memory.h>
#include <stdio.h>
#include <timer.h>


int errno = 0;


void main(BootInformation* boot_info)
{
    // Memory
    mm_initialize(boot_info);
    // IDT and IRQ
    idt_install();
    irq_install();
    // Install handlers
    timer_init();
    // Re-enable interrupts
    asm("sti");
    // Initialize IO
    io_init();

    // Welcome
    printf("Stage 2 loaded from drive %i\n", boot_info->boot_drive);

    printf("%d entries found for the memory\n", boot_info->smap_size);
    int i = 0;
    for (i = 0; i < boot_info->smap_size; ++i) {
        SMAPEntry *ent = &(boot_info->smap_entries[i]);
        printf("\tACPI Flags 0x%x Type %d ",
                ent->acpi & 0x03, ent->type);
        printf("Base 0x%016llx\tSize %lld\n", ent->base, ent->length);
    }

    // Launch command line interpreter
    CLI();

    // Should never reach this, but just in case...
    while (1)
        asm("hlt");
}
