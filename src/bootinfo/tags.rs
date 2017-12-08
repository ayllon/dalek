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
    // Does a a shield to prevent crashes when doing a match on this enum
    FirstUnknown = 11,
}

use core::fmt;
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
            t => write!(f,"Unknown tag {}", t as u32),
        }
    }
}

/// All tags contain these fields
#[repr(C)]
#[derive(Copy, Clone, Debug)]
pub struct Tag {
    /// Tag type
    pub typ: Type,
    /// Size of the full tag, including these two fields
    pub size: u32,
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

/// This tags contains the Boot loader name
#[repr(packed)]
pub struct TagBootLoaderName {
    tag: Tag,
    /// First character of the string containing the name
    pub string: u8
}

impl TagBootLoaderName {
    /// Returns a slice containing the full boot loader name
    pub fn name(&self) -> &str {
        use core::{mem, str, slice};
        let strlen = self.tag.size as usize - mem::size_of::<Tag>();
        unsafe {
            str::from_utf8_unchecked(
                slice::from_raw_parts((&self.string) as *const u8, strlen)
            )
        }
    }

    /// Casts a Tag into a TagBootLoaderName
    pub fn from(original: Option<&'static Tag>) -> Option<&'static TagBootLoaderName> {
        original.map(|t| unsafe { &*{ t as *const Tag as *const TagBootLoaderName } })
    }
}
