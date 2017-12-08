// http://nongnu.askapache.com/grub/phcoder/multiboot.pdf
#[allow(dead_code)]
#[repr(u32)]
#[derive(PartialEq)]
pub enum TagType {
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

#[repr(C)]
pub struct Tag {
    pub tag_type: TagType,
    pub size: u32,
}

#[repr(packed)]
pub struct BootLoaderName {
    tag: Tag,
    pub string: u8
}

impl BootLoaderName {
    pub fn name(&self) -> &str {
        use core::{mem,str,slice};
        let strlen = self.tag.size as usize - mem::size_of::<BootLoaderName>();
        unsafe {
            str::from_utf8_unchecked(
                slice::from_raw_parts((&self.string) as *const u8, strlen)
            )
        }
    }
}

pub struct TagIter {
    pub current: *const Tag,
}

impl Iterator for TagIter {
    type Item = &'static Tag;

    fn next(&mut self) -> Option<&'static Tag> {
        match unsafe { &*self.current } {
            &Tag { tag_type: TagType::EndTag, size: 8 } => None,
            tag => {
                let mut next_addr = (self.current as usize) + tag.size as usize;
                next_addr = ((next_addr - 1) & !0x7) + 0x8; // align
                self.current = next_addr as *const _;

                Some(tag)
            }
        }
    }
}

#[repr(C)]
pub struct MultibootInfo {
    pub total_size: u32,
    reserved: u32,
    first_tag: Tag,
}

/// Prepares the Multiboo information passed by the boot loader
/// to be used from Rust.
/// # Arguments
/// * `address` The memory address passed by the boot loader at %ebx
pub fn load(address: usize) -> &'static MultibootInfo {
    unsafe { &*(address as *const MultibootInfo) }
}

impl MultibootInfo {
    pub fn get_boot_loader_name(&self) -> Option<&'static BootLoaderName> {
        self.get_tag(TagType::BootLoaderName).map(|tag| unsafe { &*{ tag as *const Tag as *const BootLoaderName } })
    }

    /// Returns the requested tag, or None if not found
    pub fn get_tag(&self, typ: TagType) -> Option<&'static Tag> {
        self.tags().find(|tag| tag.tag_type == typ)
    }

    /// Returns an iterable with the tags
    pub fn tags(&self) -> TagIter {
        TagIter { current: &self.first_tag as *const _ }
    }
}
