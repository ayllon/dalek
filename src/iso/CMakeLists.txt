cmake_minimum_required (VERSION 2.8)

add_custom_target (dalek.iso ALL
    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/grub"
    COMMAND cp "${CMAKE_CURRENT_BINARY_DIR}/../arch/x86_64/kernel.elf" "${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/kernel.elf"
    COMMAND cp "${CMAKE_CURRENT_SOURCE_DIR}/grub.cfg" "${CMAKE_CURRENT_BINARY_DIR}/isofiles/boot/grub"
    COMMAND grub-mkrescue -o "${CMAKE_BINARY_DIR}/dalek.iso" "${CMAKE_CURRENT_BINARY_DIR}/isofiles"
    DEPENDS kernel.elf
)
