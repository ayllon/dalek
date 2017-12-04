#![feature(lang_items)]
#![feature(asm)]
#![no_std]
#![no_main]

extern crate rlibc;

#[no_mangle]
pub extern fn rust_main() {
    unsafe {
        asm!("mov $$0x2f592f412f4b2f4f, %rax");
        asm!("mov %rax, 0xb8000");
    }
}

#[lang = "eh_personality"] extern fn eh_personality() {}
#[lang = "panic_fmt"] #[no_mangle] pub extern fn panic_fmt() -> ! {loop{}}
