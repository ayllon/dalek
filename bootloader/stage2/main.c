/* Main file */
#include <types.h>
#include <printf.h>
#include <memory.h>
#include <idt.h>
#include <io.h>
#include <irq.h>
#include <timer.h>
#include <keyboard.h>
#include <cli.h>
#include <boot.h>


int errno = 0;


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
    // Initialize IO
    io_init();

    // Try reading something from floppy
    IODevice* dev = io_device_get_by_name("fd0");
    if (dev) {
        printf("Position before: %lld\n", dev->seek(dev, 0, SEEK_CUR));
        dev->seek(dev, 0, SEEK_SET);
        printf("Position after: %lld\n", dev->seek(dev, 0, SEEK_CUR));

        char buffer[512] = {0};
        if (dev->read(dev, buffer, sizeof(buffer)) >= 0)
            printf("%s\n", buffer);
        else
            log(LOG_WARN, __func__, "Could not read: %d", errno);
    }

    // Launch command line interpreter
    CLI();
}
