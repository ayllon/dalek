// http://nongnu.askapache.com/grub/phcoder/multiboot.pdf
pub mod tags;


/// Represents a Boot information struct passed by the boot loader
#[repr(C)]
pub struct BootInfo {
    pub total_size: u32,
    reserved: u32,
    first_tag: tags::Tag,
}

/// Prepares the Multiboot information passed by the boot loader
/// to be used from Rust.
/// # Arguments
/// * `address` The memory address passed by the boot loader at %ebx
pub fn load(address: usize) -> &'static BootInfo {
    unsafe { &*(address as *const BootInfo) }
}

impl BootInfo {
    /// Returns the requested tag, or None if not found
    pub fn get_tag<T: tags::TagTrait>(&self) -> Option<&'static T> {
        return self.tags()
            .find(|tag| tag.typ == T::TYPE_ID)
            .map(tags::cast_tag::<T>)
    }

    /// Returns an iterable with the tags
    pub fn tags(&self) -> tags::TagIter {
        tags::TagIter { current: &self.first_tag as *const _ }
    }
}
