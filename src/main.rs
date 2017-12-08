#![feature(asm)]
#![feature(const_fn)]
#![feature(lang_items)]
#![feature(unique)]

#![no_std]
#![no_main]

extern crate rlibc;
extern crate spin;
extern crate volatile;

mod multiboot;
mod vga_buffer;

use core::fmt::Write;

#[no_mangle]
pub extern fn rust_main(multiboot_address: usize) {
    vga_buffer::WRITER.lock().clear();

    let boot_info = multiboot::load(multiboot_address);
    write!(vga_buffer::WRITER.lock(),
           "Multiboot address: 0x{:x}, size {} bytes\n", multiboot_address, boot_info.total_size
    );

    let loader = boot_info.get_boot_loader_name();
    match loader {
        Some(tag) => write!(vga_buffer::WRITER.lock(), "Loader: {}\n", tag.name()),
        _ => write!(vga_buffer::WRITER.lock(), "Unknown loader\n"),
    };

    loop {}
}

#[lang = "eh_personality"]
extern fn eh_personality() {}

#[lang = "panic_fmt"]
#[no_mangle]
pub extern fn panic_fmt() -> ! { loop {} }
