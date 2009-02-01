/*
 * idt.h
 *
 * Interrupt Descriptor Table
 *
 */
#ifndef __IDT_H__
#define __IDT_H__

#define IDT_PRESENT 0x80
#define IDT_RING_0  0x0E
#define IDT_RING_1  0x2E
#define IDT_RING_2  0x4E
#define IDT_RING_3  0x6E

extern void _isr0();
extern void _isr1();
extern void _isr2();
extern void _isr3();
extern void _isr4();
extern void _isr5();
extern void _isr6();
extern void _isr7();
extern void _isr8();
extern void _isr9();
extern void _isr10();
extern void _isr11();
extern void _isr12();
extern void _isr13();
extern void _isr14();
extern void _isr15();
extern void _isr16();
extern void _isr17();
extern void _isr18();
extern void _isr19();
extern void _isr20();
extern void _isr21();
extern void _isr22();
extern void _isr23();
extern void _isr24();
extern void _isr25();
extern void _isr26();
extern void _isr27();
extern void _isr28();
extern void _isr29();
extern void _isr30();
extern void _isr31();

void idt_set_gate(uint8 num, uint32 base, uint16 sel, uint8 flags);
void idt_install();

#endif
