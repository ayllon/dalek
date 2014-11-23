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

void main(uint16 drive)
{
    // Welcome
    setcolor(WHITE, BLUE);
    cls();
    printf("Stage 2 loaded from drive %i\n", drive);
    // Memory
    mm_initialize();
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
