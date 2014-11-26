/*
 * idt.c
 *
 * Interrupt Descriptor Table
 */
#include <types.h>
#include <idt.h>
#include <registers.h>
#include <memory.h>
#include <printf.h>

/* IDT Entry */
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel; /* Code segment */
    uint8_t reserved; /* ALWAYS 0! */
    uint8_t flags;
    uint16_t base_hi;
}__attribute__((packed));

/* IDT Pointer */
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
}__attribute__ ((packed));


/* Declare an IDT of 256 entries.
 */
struct idt_entry idt[256];
struct idt_ptr   idtp;


/* Load IDT
*/
void idt_load()
{
    asm("lidt idtp");
}

/* Use this function to set an entry in the IDT. */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt[num].base_lo = (base & 0xFFFF);
    idt[num].base_hi = (base >> 16) & 0xFFFF;
    idt[num].reserved = 0;
    idt[num].flags = flags;
    idt[num].sel = sel;
}

/* Installs the IDT */
void idt_install()
{
    /* Sets the special IDT pointer up, just like in 'gdt.c' */
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t) (&idt);

    /* Clear out the entire IDT, initializing it to zeros */
    memset((void*) &idt, 0, sizeof(struct idt_entry) * 256);

    /* Set base 18 */
    idt_set_gate(0, (uint32_t) _isr0, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(1, (uint32_t) _isr1, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(2, (uint32_t) _isr2, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(3, (uint32_t) _isr3, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(4, (uint32_t) _isr4, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(5, (uint32_t) _isr5, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(6, (uint32_t) _isr6, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(7, (uint32_t) _isr7, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(8, (uint32_t) _isr8, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(9, (uint32_t) _isr9, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(10, (uint32_t) _isr10, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(11, (uint32_t) _isr11, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(12, (uint32_t) _isr12, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(13, (uint32_t) _isr13, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(14, (uint32_t) _isr14, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(15, (uint32_t) _isr15, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(16, (uint32_t) _isr16, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(17, (uint32_t) _isr17, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(18, (uint32_t) _isr18, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(19, (uint32_t) _isr19, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(20, (uint32_t) _isr20, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(21, (uint32_t) _isr21, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(22, (uint32_t) _isr22, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(23, (uint32_t) _isr23, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(24, (uint32_t) _isr24, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(25, (uint32_t) _isr25, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(26, (uint32_t) _isr26, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(27, (uint32_t) _isr27, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(28, (uint32_t) _isr28, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(29, (uint32_t) _isr29, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(30, (uint32_t) _isr30, 0x08, IDT_PRESENT | IDT_RING_0);
    idt_set_gate(31, (uint32_t) _isr31, 0x08, IDT_PRESENT | IDT_RING_0);

    /* Points the processor's internal register to the new IDT */
    idt_load();
}


/** Base exception handler**/

static char *isr_exception_string[] =
{
   "Division By Zero Exception",
   "Debug Exception",
   "Non Maskable Interrupt Exception",
   "Breakpoint Exception",
   "Into Detected Overflow Exception",
   "Out of Bounds Exception",
   "Invalid Opcode Exception",
   "No Coprocessor Exception",
   "Double Fault Exception",
   "Coprocessor Segment Overrun Exception",
   "Bad TSS Exception",
   "Segment Not Present Exception",
   "Stack Fault Exception",
   "General Protection Fault Exception",
   "Page Fault Exception",
   "Unknown Interrupt Exception",
   "Coprocessor Fault Exception",
   "Alignment Check Exception (486+)",
   "Machine Check Exception"
};

void isr_base_handler(Registers *r)
{
    if (r->int_no < 19) {
        printf("[isr_base_handler(%i, %i)] %s\n", r->int_no, r->err_code,
                isr_exception_string[r->int_no]);
    }
    else {
        printf("[isr_base_handler(%i, %i)] Reserved exception\n", r->int_no,
                r->err_code);
    }
}
