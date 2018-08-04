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

fn _print_memory_map(memory_map: &'static bootinfo::tags::MemoryMap) {
    println!("\nMemory map version {}", memory_map.entry_version);
    let mem_iter = memory_map.into_iter();
    for mem in mem_iter {
        println!("{:?}", mem);
    }
}

#[no_mangle]
pub extern fn rust_main(multiboot_address: usize) {
    vga_buffer::WRITER.lock().clear();

    let boot_info = bootinfo::load(multiboot_address);
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

    boot_info.get_tag::<bootinfo::tags::Framebuffer>()
        .map(|f| println!("Framebuffer: {:?}", f));

    panic!();
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
        unsafe {
            asm!("HLT");
        }
    }
}
