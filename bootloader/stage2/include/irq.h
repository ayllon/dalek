/*
 * irq.h
 *
 * Interrupt Requests
 */
#ifndef __IRQ_H__
#define __IRQ_H__

#include <registers.h>
#include <types.h>

#define PIC_MASTER_PORT_A  0x20
#define PIC_MASTER_PORT_B  0x21
#define PIC_SLAVE_PORT_A   0xA0
#define PIC_SLAVE_PORT_B   0xA1

extern void _irq0();
extern void _irq1();
extern void _irq2();
extern void _irq3();
extern void _irq4();
extern void _irq5();
extern void _irq6();
extern void _irq7();
extern void _irq8();
extern void _irq9();
extern void _irq10();
extern void _irq11();
extern void _irq12();
extern void _irq13();
extern void _irq14();
extern void _irq15();

void irq_install();
void irq_install_handler(uint8_t irq, void (*handler)(Registers*));
void irq_uninstall_handler(uint8_t irq);

#endif
