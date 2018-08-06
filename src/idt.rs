use core::fmt;

// https://wiki.osdev.org/Exceptions
pub const DIVIDE_BY_ZERO: u8 = 0x0;
pub const DEBUG: u8 = 0x01;
pub const NON_MASKABLE_INTERRUPT: u8 = 0x02;
pub const BREAKPOINT: u8 = 0x03;
pub const OVERFLOW: u8 = 0x04;
pub const INVALID_OPCODE: u8 = 0x06;
pub const DOUBLE_FAULT: u8 = 0x08;
pub const SEGMENT_NOT_PRESENT: u8 = 0x0B;
pub const STACK_SEGMENT_FAULT: u8 = 0x0C;
pub const GENERAL_PROTECTION_FAULT: u8 = 0x0D;
pub const PAGE_FAULT: u8 = 0x0E;
pub const ALIGNMENT_CHECK: u8 = 0x11;

#[derive(Debug)]
pub struct ExceptionStackFrame {
    pub instruction_pointer: u64,
    pub code_segment: u64,
    pub cpu_flags: u64,
    pub stack_pointer: u64,
    pub stack_segment: u64,
}

pub type HandlerFunc = extern "x86-interrupt" fn(&ExceptionStackFrame);


#[derive(Debug, Clone, Copy)]
pub struct SegmentSelector(u16);

impl SegmentSelector {
    fn new(segment: u16, ring: u8) -> SegmentSelector {
        SegmentSelector(segment+ring as u16)
    }
}

#[derive(Clone, Copy)]
pub struct Options(u16);

impl Options {
    fn minimal() -> Self {
        Options(0b0000111000000000)
    }

    fn new() -> Self {
        let mut options = Self::minimal();
        options.set_present(true).enable_interrupt(false);
        options
    }

    pub fn set_stack(&mut self, stack: u8) -> &mut Self {
        self.0 = self.0 & 0xFFF8 | stack as u16;
        self
    }

    pub fn enable_interrupt(&mut self, set: bool) -> &mut Self {
        self.0 = self.0 & 0xFF7F | if set { 1 << 8 } else { 0 };
        self
    }

    pub fn set_present(&mut self, present: bool) -> &mut Self {
        self.0 = self.0 & 0x7FFF | if present { 1 << 15 } else { 0 };
        self
    }
}

impl fmt::Debug for Options {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "0x{:04X}", self.0)
    }
}

#[derive(Clone, Copy)]
#[repr(C)]
pub struct Entry {
    ptr_low: u16,
    gdt_selector: SegmentSelector,
    options: Options,
    ptr_mid: u16,
    ptr_high: u32,
    reserved: u32,
}

impl Entry {
    fn missing() -> Entry {
        Entry {
            ptr_low: 0, ptr_mid: 0, ptr_high: 0,
            gdt_selector: SegmentSelector::new(0, 0),
            options: Options::minimal(),
            reserved: 0
        }
    }

    fn new(selector: SegmentSelector, handler: HandlerFunc) -> Entry {
        let ptr = handler as u64;
        Entry {
            gdt_selector: selector,
            ptr_low: ptr as u16,
            ptr_mid: (ptr >> 16) as u16,
            ptr_high: (ptr >> 32) as u32,
            options: Options::new(),
            reserved: 0
        }
    }
}

impl fmt::Debug for Entry {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f,
            "{{gdt_selector: {:?}, ptr_low: 0x{:04X}, ptr_mid: 0x{:04X}, ptr_high: 0x{:08X}, options: {:?}}}",
            self.gdt_selector, self.ptr_low, self.ptr_mid, self.ptr_high, self.options
        )
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Idt([Entry; 16]);

impl Idt {
    pub fn new() -> Idt {
        Idt([Entry::missing();16])
    }

    pub fn set_handler(&mut self, entry: u8, handler: HandlerFunc) -> &mut Options {
        self.0[entry as usize] = Entry::new(SegmentSelector::new(8, 0), handler);
        &mut self.0[entry as usize].options
    }

    pub fn get_handler(&self, entry: u8) -> Entry {
        self.0[entry as usize]
    }

    pub fn load(&self) {
        use core::mem::size_of;

        #[repr(C,packed)]
        struct IdtAddress {
            limit: u16,
            offset: *const Idt,
        }

        let idt_address = IdtAddress {
            limit: (size_of::<Self>() - 1) as u16,
            offset: self
        };

        unsafe {
            asm!{"lidt ($0)" :: "r"(&idt_address) : "memory"};
        }
    }
}
