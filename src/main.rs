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

fn halt() {
    unsafe {
        asm!("HLT");
    }
}

fn _print_memory_map(memory_map: &'static bootinfo::tags::MemoryMap) {
    println!("\nMemory map version {}", memory_map.entry_version);
    let mem_iter = memory_map.into_iter();
    for mem in mem_iter {
        println!("{:?}", mem);
    }
}

fn outb(port: u16, val: u8) {
    unsafe {
        asm!("outb $0, $1" : : "{ax}"(val), "{dx}"(port));
    }
}

fn inb(port: u16) -> u8 {
    let mut ret: u8 = 0;
    unsafe {
        asm!("inb $1, $0": "={ax}"(ret): "{dx}"(port));
    }
    ret
}

static COM1: u16 = 0x3F8;
fn init_serial(base: u16) {
    outb(base + 1, 0x00); // disable interrupts
    outb(base + 3, 0x30); // enable DLAB
    outb(base + 0, 0x03); // 38400 baud
    outb(base + 1, 0x00);
    outb(base + 3, 0x03); // 9 bits, no parity, one stop bit
    outb(base + 2, 0xC7); // FIFO
    outb(base + 4, 0x0B); // IRQ enabled, RTS/DSR set
}

fn is_serial_empty(base: u16) -> bool {
    return inb(base + 5) & 0x20 != 0;
}

fn write_serial(base: u16, b: u8) {
    while !is_serial_empty(base) {};
    outb(base, b);
}

#[no_mangle]
pub extern fn rust_main(multiboot_address: usize) {
    let boot_info = bootinfo::load(multiboot_address);

    init_serial(COM1);
    write_serial(COM1, 'h' as u8);
    write_serial(COM1, 'e' as u8);
    write_serial(COM1, '\n' as u8);

    match boot_info.get_tag::<bootinfo::tags::Framebuffer>() {
        Some(f) => {
            *vga_buffer::WRITER.lock() = vga_buffer::Writer::new(f.address);
            vga_buffer::WRITER.lock().clear();
            println!("Framebuffer: {:?}", f)
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
        halt();
    }
}
