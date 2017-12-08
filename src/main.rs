#![feature(asm)]
#![feature(const_fn)]
#![feature(lang_items)]
#![feature(unique)]

#![no_std]
#![no_main]

extern crate rlibc;
extern crate spin;
extern crate volatile;

mod bootinfo;
#[macro_use]
mod vga_buffer;

#[no_mangle]
pub extern fn rust_main(multiboot_address: usize) {
    vga_buffer::WRITER.lock().clear();

    let boot_info = bootinfo::load(multiboot_address);
    println!("Multiboot address: 0x{:x}, size {} bytes", multiboot_address, boot_info.total_size);

    for tag in boot_info.tags() {
        println!("{:?}", tag);
    }

    panic!();
}

#[lang = "eh_personality"]
extern fn eh_personality() {}

#[lang = "panic_fmt"]
#[no_mangle]
pub extern fn panic_fmt(fmt: core::fmt::Arguments, file: &'static str, line: u32) -> ! {
    println!("PANIC! {}:{}", file, line);
    println!("  {}", fmt);
    loop {}
}
