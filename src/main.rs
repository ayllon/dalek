#![feature(asm)]
#![feature(const_fn)]
#![feature(lang_items)]
#![feature(panic_implementation)]
#![feature(panic_info_message)]
#![feature(ptr_internals)]

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
    let loader = boot_info
        .get_tag(bootinfo::tags::Type::BootLoaderName)
        .map(bootinfo::tags::BootLoaderName::cast);

    println!("Multiboot address: 0x{:x}, size {} bytes", multiboot_address, boot_info.total_size);
    loader.map(|loader| println!("Booted by {}", loader.name()));

    for tag in boot_info.tags() {
        println!("{:?}", tag);
    }

    let bat = boot_info.get_tag(bootinfo::tags::Type::ImageLoadBaseAddress);
    let ba = bat.map(bootinfo::tags::ImageLoadBaseAddress::cast);
    println!("{:x}", ba.unwrap().load_base_address);

    panic!();
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

    loop {}
}
