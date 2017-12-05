#![feature(lang_items)]
#![feature(asm)]
#![no_std]
#![no_main]

extern crate rlibc;

#[no_mangle]
pub extern fn rust_main() {
    let hello = b"Hello there!";
    let color = 0x1f;

    let mut hello_colored = [color; 24];
    for (i, char) in hello.into_iter().enumerate() {
        hello_colored[i * 2] = *char;
    }

    let buffer_ptr = (0xb8000 + 1988) as *mut _;
    unsafe {
        *buffer_ptr = hello_colored;
    }

    loop {}
}

#[lang = "eh_personality"]
extern fn eh_personality() {}

#[lang = "panic_fmt"]
#[no_mangle]
pub extern fn panic_fmt() -> ! { loop {} }
