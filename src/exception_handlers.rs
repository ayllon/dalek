pub const DIVIDE_BY_ZERO: u8 = 0x0;
pub const OVERFLOW: u8 = 0x04;
pub const INVALID_OPCODE: u8 = 0x06;
pub const DOUBLE_FAULT: u8 = 0x08;
pub const SEGMENT_NOT_PRESENT: u8 = 0x0B;
pub const STACK_SEGMENT_FAULT: u8 = 0x0C;
pub const GENERAL_PROTECTION_FAULT: u8 = 0x0D;
pub const PAGE_FAULT: u8 = 0x0E;
pub const ALIGNMENT_CHECK: u8 = 0x11;

use idt;

pub extern "x86-interrupt" fn divide_by_zero(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Divide by zero!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn overflow(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Overflow!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn invalid_opcode(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Invalid opcode!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn double_fault(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Double fault!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn segment_not_present(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Segment not present!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn stack_segment_fault(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Stack segment fault!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn general_protection_fault(stack_frame: &idt::ExceptionStackFrame) {
    panic!("General protection fault!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn page_fault(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Page fault!\n{:?}", stack_frame);
}

pub extern "x86-interrupt" fn alignment_check(stack_frame: &idt::ExceptionStackFrame) {
    panic!("Alignment failure!\n{:?}", stack_frame);
}
