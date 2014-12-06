/* Main file */
#include <types.h>
#include <printf.h>
#include <memory.h>
#include <idt.h>
#include <irq.h>
#include <timer.h>
#include <keyboard.h>
#include <cli.h>
#include <floppy.h>
#include <boot.h>


void main(BootInformation* boot_info)
{
    // Welcome
    setcolor(WHITE, BLUE);
    cls();
    printf("Stage 2 loaded from drive %i\n", boot_info->boot_drive);

    printf("%d entries found for the memory\n", boot_info->smap_size);
    int i = 0;
    for (i = 0; i < boot_info->smap_size; ++i) {
        SMAPEntry *ent = &(boot_info->smap_entries[i]);
        printf("\tACPI Flags 0x%x Type %d ",
                ent->acpi & 0x03, ent->type);
        printf("Base 0x%016llx\tSize %lld\n", ent->base, ent->length);
    }

    // Memory
    mm_initialize(boot_info);
    // IDT and IRQ
    idt_install();
    irq_install();
    // Install handlers
    timer_init();
    kb_init();
    // Re-enable interrupts
    asm("sti");
    // Initialize drivers
    fd_init();

    // Launch command line interpreter
    CLI();
}
