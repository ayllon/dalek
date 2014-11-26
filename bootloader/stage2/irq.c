/*
 * irq.c
 *
 * Interrupt Requests
 *
 */
#include <irq.h>
#include <types.h>
#include <idt.h>
#include <ports.h>
#include <printf.h>

/* IRQ Handlers */
static void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Install a handler
 */
void irq_install_handler(uint8_t irq, void (*handler)(Registers *r))
{
    irq_routines[irq] = handler;
}

/**
 * Clear a handler
 */
void irq_uninstall_handler(uint8_t irq)
{
    irq_routines[irq] = NULL;
}

/**
 * Remap IRQs to IDT 32 to 47
 */
void irq_remap()
{
    int a1, a2;
    a1 = inportb(0x20);
    a2 = inportb(0xA0);

    outportb(0x20, 0x11);
    outportb(0xA0, 0x11);
    outportb(0x21, 0x20);
    outportb(0xA1, 0x28);
    outportb(0x21, 0x04);
    outportb(0xA1, 0x02);
    outportb(0x21, 0x01);
    outportb(0xA1, 0x01);

    outportb(0x21, a1);
    outportb(0xA1, a2);
}

/**
 */
void irq_install()
{
    irq_remap();
    idt_set_gate(32, (uint32_t) _irq0, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(33, (uint32_t) _irq1, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(34, (uint32_t) _irq2, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(35, (uint32_t) _irq3, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(36, (uint32_t) _irq4, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(37, (uint32_t) _irq5, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(38, (uint32_t) _irq6, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(39, (uint32_t) _irq7, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(40, (uint32_t) _irq8, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(41, (uint32_t) _irq9, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(42, (uint32_t) _irq10, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(43, (uint32_t) _irq11, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(44, (uint32_t) _irq12, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(45, (uint32_t) _irq13, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(46, (uint32_t) _irq14, 0x08, IDT_RING_0 | IDT_PRESENT);
    idt_set_gate(47, (uint32_t) _irq15, 0x08, IDT_RING_0 | IDT_PRESENT);
}

/**
 */
void irq_handler(Registers *r)
{
    void (*handler)(Registers *r);

    handler = irq_routines[r->int_no - 32];
    if (handler) {
        handler(r);
    }
    else {
        printf("[irq_handler(%i, %i)] Interrupt without handler. Ignore.\n",
                r->int_no - 32, r->err_code);
    }

    outportb(0x20, 0x20);
}
