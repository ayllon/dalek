#![feature(asm)]
#![feature(const_fn)]
#![feature(lang_items)]
#![feature(unique)]

#![no_std]
#![no_main]

extern crate rlibc;
extern crate spin;
extern crate volatile;

mod vga_buffer;

#[no_mangle]
pub extern fn rust_main() {
    use core::fmt::Write;
    vga_buffer::WRITER.lock().clear();
    vga_buffer::WRITER.lock().write_str("Hello!");
    write!(vga_buffer::WRITER.lock(), ", bididi {}\n", 42);
    write!(vga_buffer::WRITER.lock(), "Petete");
    loop {}
}

#[lang = "eh_personality"]
extern fn eh_personality() {}

#[lang = "panic_fmt"]
#[no_mangle]
pub extern fn panic_fmt() -> ! { loop {} }
