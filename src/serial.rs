pub static COM1: u16 = 0x3F8;
pub static COM2: u16 = 0x2F8;

use arch::{inb, outb};

pub struct Serial {
    port: u16,
}

impl Serial {
    pub fn new(port: u16) -> Serial {
        let s = Serial { port };
        s
    }

    fn init(&self) {
        outb(self.port + 1, 0x00); // disable interrupts
        outb(self.port + 3, 0x30); // enable DLAB
        outb(self.port + 0, 0x03); // 38400 baud
        outb(self.port + 1, 0x00);
        outb(self.port + 3, 0x03); // 8 bits, no parity, one stop bit
        outb(self.port + 2, 0xC7); // FIFO
        outb(self.port + 4, 0x0B); // IRQ enabled, RTS/DSR set
    }

    fn is_fifo_empty(&self) -> bool {
        inb(self.port + 5) & 0x20 != 0
    }

    fn write_byte(&self, b: u8) {
        while !self.is_fifo_empty() {};
        outb(self.port, b);
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
