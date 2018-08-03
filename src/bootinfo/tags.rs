use core::{fmt, mem, str, slice};

/// Known tag types
#[allow(dead_code)]
#[repr(u32)]
#[derive(Copy, Clone, PartialEq)]
pub enum Type {
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

impl fmt::Debug for Type {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        match *self {
            Type::EndTag => f.write_str("EndTag"),
            Type::CommandLine => f.write_str("CommandLine"),
            Type::BootLoaderName => f.write_str("BootLoaderName"),
            Type::Modules => f.write_str("Modules"),
            Type::MemoryInformation => f.write_str("MemoryInformation"),
            Type::BiosBootDevice => f.write_str("BiosBootDevice"),
            Type::MemoryMap => f.write_str("MemoryMap"),
            Type::VBEInfo => f.write_str("VBEInfo"),
            Type::FrameBufferInfo => f.write_str("FrameBufferInfo"),
            Type::ElfSymbols => f.write_str("ElfSymbols"),
            Type::APMTable => f.write_str("APMTable"),
            Type::RSDP => f.write_str("RSDP"),
            Type::RSDPv2 => f.write_str("RSDPv2"),
            Type::ImageLoadBaseAddress => f.write_str("ImageLoadBaseAddress"),
            t => write!(f,"Unknown tag {}", t as u32),
        }
    }
}

/// All tags contain these fields
#[derive(Debug)]
#[repr(C)]
pub struct Tag {
    /// Tag type
    pub typ: Type,
    /// Size of the full tag, including these two fields
    pub size: u32,
}


/// Generic implementation to convert to a known Tag type.
/// It just casts the pointer.
pub trait SpecificTag<T> {
    fn cast(original: &'static Tag) -> &'static T;
}

macro_rules! implement_cast_from_tag {
    () => {
        pub fn cast(original: &'static Tag) -> &'static Self {
            unsafe { &*{ original as *const Tag as *const Self }}
        }
    };
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
            &Tag { typ: Type::EndTag, size: 8 } => None,
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
    implement_cast_from_tag!();

    pub fn cmd(&self) -> &str {
        let strlen = self.tag.size as usize - mem::size_of::<Tag>();
        unsafe {
            str::from_utf8_unchecked(
                slice::from_raw_parts((&self.string) as *const u8, strlen)
            )
        }
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

impl BootLoaderName {
    implement_cast_from_tag!();

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

impl ImageLoadBaseAddress {
    implement_cast_from_tag!();
}
