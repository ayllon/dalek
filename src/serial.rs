pub static COM1: u16 = 0x3F8;
pub static COM2: u16 = 0x2F8;

fn _outb(port: u16, val: u8) {
    unsafe {
        asm!("outb $0, $1" : : "{ax}"(val), "{dx}"(port));
    }
}

fn _inb(port: u16) -> u8 {
    let mut ret: u8 = 0;
    unsafe {
        asm!("inb $1, $0": "={ax}"(ret): "{dx}"(port));
    }
    ret
}

pub struct Serial {
    port: u16,
}

impl Serial {
    pub fn new(port: u16) -> Serial {
        let s = Serial { port };
        s
    }

    fn init(&self) {
        _outb(self.port + 1, 0x00); // disable interrupts
        _outb(self.port + 3, 0x30); // enable DLAB
        _outb(self.port + 0, 0x03); // 38400 baud
        _outb(self.port + 1, 0x00);
        _outb(self.port + 3, 0x03); // 8 bits, no parity, one stop bit
        _outb(self.port + 2, 0xC7); // FIFO
        _outb(self.port + 4, 0x0B); // IRQ enabled, RTS/DSR set
    }

    fn is_fifo_empty(&self) -> bool {
        return _inb(self.port + 5) & 0x20 != 0;
    }

    fn write_byte(&self, b: u8) {
        while !self.is_fifo_empty() {};
        _outb(self.port, b);
    }
}

use core::fmt;
impl fmt::Write for Serial {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        for byte in s.bytes() {
            self.write_byte(byte);
        }
        Ok(())
    }

    fn write_char(&mut self, c: char) -> fmt::Result {
        self.write_byte(c as u8);
        Ok(())
    }
}
