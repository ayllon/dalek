use core::{fmt, mem, str, slice};

/// Known tag types
#[allow(dead_code)]
#[repr(u32)]
#[derive(Copy, Clone, PartialEq)]
pub enum TypeId {
    EndTag = 0,
    CommandLine = 1,
    BootLoaderName = 2,
    Modules = 3,
    MemoryInformation = 4,
    BiosBootDevice = 5,
    MemoryMap = 6,
    VBEInfo = 7,
    FrameBufferInfo = 8,
    ElfSymbols = 9,
    APMTable = 10,
    RSDP = 14,
    RSDPv2 = 15,
    ImageLoadBaseAddress = 21,
    // Needed to make sure the compiler does not optimize away the "default" match
    // for unknown values, and crash when there is one
    Barrier,
}

impl fmt::Debug for TypeId {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            TypeId::EndTag => f.write_str("EndTag"),
            TypeId::CommandLine => f.write_str("CommandLine"),
            TypeId::BootLoaderName => f.write_str("BootLoaderName"),
            TypeId::Modules => f.write_str("Modules"),
            TypeId::MemoryInformation => f.write_str("MemoryInformation"),
            TypeId::BiosBootDevice => f.write_str("BiosBootDevice"),
            TypeId::MemoryMap => f.write_str("MemoryMap"),
            TypeId::VBEInfo => f.write_str("VBEInfo"),
            TypeId::FrameBufferInfo => f.write_str("FrameBufferInfo"),
            TypeId::ElfSymbols => f.write_str("ElfSymbols"),
            TypeId::APMTable => f.write_str("APMTable"),
            TypeId::RSDP => f.write_str("RSDP"),
            TypeId::RSDPv2 => f.write_str("RSDPv2"),
            TypeId::ImageLoadBaseAddress => f.write_str("ImageLoadBaseAddress"),
            t => write!(f,"Unknown tag {}", t as u32),
        }
    }
}

/// All tags contain these fields
#[derive(Debug)]
#[repr(C)]
pub struct Tag {
    /// Tag type
    pub typ: TypeId,
    /// Size of the full tag, including these two fields
    pub size: u32,
}

pub trait TagTrait {
    const TYPE_ID: TypeId;
}

/// Cast pointer from a generic Tag to a specific one
pub fn cast_tag<T>(original: &'static Tag) -> &'static T {
    unsafe { &*{ original as *const Tag as *const T }}
}


/// Iterates through tags in memory
pub struct TagIter {
    pub current: *const Tag,
}

/// Implements the Iterator trait for TagIter
impl Iterator for TagIter {
    type Item = &'static Tag;

    /// Gives next tag, None if reached the end of list
    fn next(&mut self) -> Option<&'static Tag> {
        match unsafe { &*self.current } {
            &Tag { typ: TypeId::EndTag, size: 8 } => None,
            tag => {
                let mut next_addr = (self.current as usize) + tag.size as usize;
                next_addr = ((next_addr - 1) & !0x7) + 0x8; // align
                self.current = next_addr as *const _;

                Some(tag)
            }
        }
    }
}

/// This tag contains the command line passed by the boot loader
#[derive(Debug)]
#[repr(C)]
pub struct CommandLine {
    tag: Tag,
    /// First character of the command line
    pub string: u8
}

impl CommandLine {
    pub fn cmd(&self) -> &str {
        let strlen = self.tag.size as usize - mem::size_of::<Tag>();
        unsafe {
            str::from_utf8_unchecked(
                slice::from_raw_parts((&self.string) as *const u8, strlen)
            )
        }
    }
}

impl TagTrait for CommandLine {
    const TYPE_ID: TypeId = TypeId::CommandLine;
}

/// This tag contains the boot device used by the BIOS
#[derive(Debug)]
#[repr(C)]
pub struct BiosBootDevice {
    tag: Tag,
    pub biosdev: u32,
    pub partition: u32,
    pub sub_partition: u32
}

impl TagTrait for BiosBootDevice {
    const TYPE_ID: TypeId = TypeId::BiosBootDevice;
}

impl fmt::Display for BiosBootDevice {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "0x{:X} 0x{:X} / 0x{:X}", self.biosdev, self.partition, self.sub_partition)
    }
}

/// This tag contains the Boot loader name
#[derive(Debug)]
#[repr(C)]
pub struct BootLoaderName {
    tag: Tag,
    /// First character of the string containing the name
    pub string: u8
}

impl TagTrait for BootLoaderName {
    const TYPE_ID: TypeId = TypeId::BootLoaderName;
}

impl BootLoaderName {
    /// Returns a slice containing the full boot loader name
    pub fn name(&self) -> &str {
        let strlen = self.tag.size as usize - mem::size_of::<Tag>();
        unsafe {
            str::from_utf8_unchecked(
                slice::from_raw_parts((&self.string) as *const u8, strlen)
            )
        }
    }
}

/// This tag contains the physical address where the image has been loaded
#[derive(Debug)]
#[repr(C)]
pub struct ImageLoadBaseAddress {
    tag: Tag,
    /// Base physical address
    pub load_base_address: u32,
}

impl TagTrait for ImageLoadBaseAddress {
    const TYPE_ID: TypeId = TypeId::ImageLoadBaseAddress;
}
