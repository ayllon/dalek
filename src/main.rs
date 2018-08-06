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
#[macro_use]
mod vga_buffer;
mod serial;
mod arch;
mod idt;
mod exception_handlers;


fn _print_memory_map(memory_map: &'static bootinfo::tags::MemoryMap) {
    println!("\nMemory map version {}", memory_map.entry_version);
    let mem_iter = memory_map.into_iter();
    for mem in mem_iter {
        println!("{:?}", mem);
    }
}

use core::fmt::Write;
use arch::halt;

#[no_mangle]
pub extern fn rust_main(multiboot_address: usize) {
    let boot_info = bootinfo::load(multiboot_address);

    let mut serial = serial::Serial::new(serial::COM1);
    write!(serial, "Hello world\n");

    match boot_info.get_tag::<bootinfo::tags::Framebuffer>() {
        Some(bootinfo::tags::Framebuffer {typ: bootinfo::tags::FramebufferType::EGA, address, ..}) => {
            *vga_buffer::WRITER.lock() = vga_buffer::Writer::new(*address);
            vga_buffer::WRITER.lock().clear();
        }
        Some(f) => {
            write!(serial, "Framebuffer: {:?}\n", f);
            let p: *const u8 = (f.address) as *const _;
            write!(serial, "Framebuffer: {:012X}\n", unsafe{*p});
            halt();
        }
        None => halt()
    }


    /*
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
    */

    IDT.load();
    println!("Installed IDT");
    println!("{:?}", IDT.get_handler(0));

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
    vga_buffer::WRITER.lock().set_color(
        vga_buffer::ColorCode::new(
            vga_buffer::Color::LightRed,
            vga_buffer::Color::Black
        )
    );

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
