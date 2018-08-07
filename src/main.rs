#![feature(asm)]
#![feature(const_fn)]
#![feature(lang_items)]
#![feature(panic_implementation)]
#![feature(panic_info_message)]
#![feature(ptr_internals)]
#![feature(abi_x86_interrupt)]

#![no_std]
#![no_main]

extern crate rlibc;
extern crate spin;
extern crate volatile;
#[macro_use]
extern crate lazy_static;

mod bootinfo;
mod vga_buffer;
mod serial;
mod arch;
mod idt;
mod exception_handlers;
#[macro_use]
mod log;

use core::fmt;
use core::mem;
use core::convert::Into;
use arch::halt;

fn _print_memory_map(memory_map: &'static bootinfo::tags::MemoryMap) {
    println!("\nMemory map version {}", memory_map.entry_version);
    let mem_iter = memory_map.into_iter();
    for mem in mem_iter {
        println!("{:?}", mem);
    }
}


#[no_mangle]
pub extern fn rust_main(multiboot_address: usize) {
    // Give some feedback ASAP, via COM1
    let mut serial = serial::Serial::new(serial::COM1);
    let serial_write: &mut fmt::Write = &mut serial;
    unsafe { log::set_writer(mem::transmute(serial_write)) };

    println!("Hello world!\n");

    // Install the base IDT soon-ish, so we can get the errors
    IDT.load();
    println!("IDT installed\n");

    let boot_info = bootinfo::load(multiboot_address);

    // Hold the VGA struct on the stack for the moment being
    let mut vga = vga_buffer::Vga::new(0x00);

    match boot_info.get_tag::<bootinfo::tags::Framebuffer>() {
        Some(f) => {
            if f.typ == bootinfo::tags::FramebufferType::EGA {
                vga = vga_buffer::Vga::new(f.address);
                unsafe { log::set_writer(mem::transmute(&mut vga as &mut fmt::Write)) };
            }
            println!("Framebuffer: {:?}\n", f);

            let p: *mut u8 = (f.address) as *mut _;
            unsafe{*p = 0xFF};
        }
        None => halt()
    }

    println!("Multiboot address: 0x{:X}, size {} bytes", multiboot_address, boot_info.total_size);

    boot_info.get_tag::<bootinfo::tags::BootLoaderName>().map(
        |loader| println!("Booted by {}", loader.name())
    );

    boot_info.get_tag::<bootinfo::tags::CommandLine>()
        .map(|cmd| println!("Command line: {}", cmd.cmd()));

    boot_info.get_tag::<bootinfo::tags::ImageLoadBaseAddress>()
        .map(|ba| println!("Running on base address {:x}", ba.load_base_address));

    boot_info.get_tag::<bootinfo::tags::BiosBootDevice>()
        .map(|dev| println!("Booted from {}", dev));

    for tag in boot_info.tags() {
        println!("{:?}", tag);
    }

    boot_info.get_tag::<bootinfo::tags::MemoryMap>()
        .map(_print_memory_map);

    unsafe{asm!("movw 0, %dx; divw %dx":::"ax", "dx")};

    panic!();
}


lazy_static! {
    static ref IDT: idt::Idt = {
        use exception_handlers::{*};

        let mut idt = idt::Idt::new();
        idt.set_handler(idt::DIVIDE_BY_ZERO, divide_by_zero);
        idt.set_handler(idt::OVERFLOW, overflow);
        idt.set_handler(idt::INVALID_OPCODE, invalid_opcode);
        idt.set_handler(idt::DOUBLE_FAULT, double_fault);
        idt.set_handler(idt::SEGMENT_NOT_PRESENT, segment_not_present);
        idt.set_handler(idt::STACK_SEGMENT_FAULT, stack_segment_fault);
        idt.set_handler(idt::GENERAL_PROTECTION_FAULT, general_protection_fault);
        idt.set_handler(idt::PAGE_FAULT, page_fault);
        idt
    };
}


#[panic_implementation]
#[no_mangle]
pub extern fn panic_fmt(info: &core::panic::PanicInfo) -> ! {
    match info.location() {
        Some(l) =>
            println!("PANIC! {}:{}", l.file(), l.line()),
        None =>
            println!("PANIC! Unknown location"),
    }

    match info.message() {
        Some(f) =>
            println!("  {}", f),
        None =>
            println!("No message??")
    }

    loop {
        halt();
    }
}
