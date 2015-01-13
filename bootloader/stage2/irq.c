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
#include <stdio.h>


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
    a1 = inportb(PIC_MASTER_PORT_A);
    a2 = inportb(PIC_SLAVE_PORT_A);

    /* See http://www.acm.uiuc.edu/sigops/roll_your_own/i386/irq.html */

    /* ICW1
     * 0 | 0 | 0 | 1 | Trigger | 0 | Master/slave config |  There will be ICW4
     * Set to 00010001 => Edge triggered, Master/Slave configuration, There will be ICW4 */
    outportb(PIC_MASTER_PORT_A, 0x11);
    outportb(PIC_SLAVE_PORT_A,  0x11);

    /* ICW2
     * Offset into the IDT, last 3 bits always 0
     * This is effectively moving the master to entry 32 to 39, and slave
     * from 40 to 47 */
    outportb(PIC_MASTER_PORT_B, 0x20);
    outportb(PIC_SLAVE_PORT_B,  0x28);

    /* ICW3
     * Bit N set to 1 if IR line N is connected to slave */
    outportb(PIC_MASTER_PORT_B, 0x04);
    /* Last three bits: IRQ on master this slave is connected to */
    outportb(PIC_SLAVE_PORT_B,  0x02);

    /* ICW4
     * 0 | 0 | 0 | Special fully nested mode | Buffered | Master/Slave  | Automatic EOI | Mode
     * Set to 0b00000101 => Master PIC, 8086/88 mode  */
    outportb(PIC_MASTER_PORT_B, 0x05);
    /* Same thing for Slave PIC */
    outportb(PIC_SLAVE_PORT_B,  0x01);

    outportb(PIC_MASTER_PORT_B, a1);
    outportb(PIC_SLAVE_PORT_B,  a2);
}


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

    log(LOG_INFO, __func__, "IRQ installed");
}


void irq_handler(Registers r, unsigned int_no, unsigned err_code,
        unsigned eip, unsigned cs, unsigned eflags)
{
    void (*handler)(Registers *r);

    handler = irq_routines[int_no - 32];
    if (handler) {
        handler(&r);
    }
    else {
        printf("[irq_handler(%i, %i)] Interrupt without handler. Ignore.\n",
                int_no - 32, err_code);
    }

    /* End Of Interrupt */
    outportb(PIC_MASTER_PORT_A, 0x20);
    outportb(PIC_MASTER_PORT_B, 0x20);
}
