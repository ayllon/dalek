/// Known tag types
#[allow(dead_code)]
#[repr(u32)]
#[derive(PartialEq)]
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
}

/// All tags contain these fields
#[repr(packed)]
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
