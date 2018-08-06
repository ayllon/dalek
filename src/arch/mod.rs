
pub fn halt() {
    unsafe {
        asm!("HLT");
    }
}

pub fn outb(port: u16, val: u8) {
    unsafe {
        asm!("outb $0, $1" : : "{ax}"(val), "{dx}"(port));
    }
}

pub fn inb(port: u16) -> u8 {
    let mut ret: u8 = 0;
    unsafe {
        asm!("inb $1, $0": "={ax}"(ret): "{dx}"(port));
    }
    ret
}
