#define ASM 1

.code32

.global _irq0
.global _irq1
.global _irq2
.global _irq3
.global _irq4
.global _irq5
.global _irq6
.global _irq7
.global _irq8
.global _irq9
.global _irq10
.global _irq11
.global _irq12
.global _irq13
.global _irq14
.global _irq15


_irq0:
	cli
	push $0
	push $32
	jmp irq_common_handler

_irq1:
	cli
	push $0
	push $33
	jmp irq_common_handler

_irq2:
	cli
	push $0
	push $34
	jmp irq_common_handler

_irq3:
	cli
	push $0
	push $35
	jmp irq_common_handler

_irq4:
	cli
	push $0
	push $36
	jmp irq_common_handler

_irq5:
	cli
	push $0
	push $37
	jmp irq_common_handler

_irq6:
	cli
	push $0
	push $38
	jmp irq_common_handler

_irq7:
	cli
	push $0
	push $39
	jmp irq_common_handler

_irq8:
	cli
	push $0
	push $40
	jmp irq_common_handler

_irq9:
	cli
	push $0
	push $41
	jmp irq_common_handler

_irq10:
	cli
	push $0
	push $42
	jmp irq_common_handler

_irq11:
	cli
	push $0
	push $43
	jmp irq_common_handler

_irq12:
	cli
	push $0
	push $44
	jmp irq_common_handler

_irq13:
	cli
	push $0
	push $45
	jmp irq_common_handler

_irq14:
	cli
	push $0
	push $46
	jmp irq_common_handler

_irq15:
	cli
	push $0
	push $47
	jmp irq_common_handler

/* IRQ Handler */
irq_common_handler:
    /* Save registers */
    pusha
    push   %cs
    push   %ds
    push   %ss
    push   %es

    /* Load the Data Segment descriptor */
    mov    $0x10, %ax
    mov    %ax, %ds
    mov    %ax, %es
    mov    %ax, %fs
    mov    %ax, %gs

    /* Call */
    call   irq_handler

    /* Recover registers */
    pop     %es
    pop     %ss
    pop     %ds
    add     $4, %esp // Note that cs can not be 'pop'
    popa

    /* Jump over interrupt number and error code */
    add $8, %esp

    /* Carry on */
    sti
    iret
