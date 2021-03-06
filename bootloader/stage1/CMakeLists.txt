cmake_minimum_required (VERSION 2.8)

add_definitions (
    "-ffreestanding -fno-stack-protector -fno-builtin -nostdinc -m32"
)

# FAT1X
add_executable (stage1-fat1x.elf fat1x.s)
set_target_properties (stage1-fat1x.elf
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -m elf_i386"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_target (stage1-fat1x.bin ALL
    COMMAND "${OBJCOPY}" -S -O binary "${CMAKE_BINARY_DIR}/stage1-fat1x.elf" "${CMAKE_BINARY_DIR}/stage1-fat1x.bin"
    COMMAND "${OBJCOPY}" --only-keep-debug "${CMAKE_BINARY_DIR}/stage1-fat1x.elf" "${CMAKE_BINARY_DIR}/stage1-fat1x.sym"
    DEPENDS stage1-fat1x.elf
)

# FAT32
add_executable (stage1-fat32.elf fat32.s)
set_target_properties (stage1-fat32.elf
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -m elf_i386"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_target (stage1-fat32.bin ALL
    COMMAND "${OBJCOPY}" -S -O binary "${CMAKE_BINARY_DIR}/stage1-fat32.elf" "${CMAKE_BINARY_DIR}/stage1-fat32.bin"
    COMMAND "${OBJCOPY}" --only-keep-debug "${CMAKE_BINARY_DIR}/stage1-fat32.elf" "${CMAKE_BINARY_DIR}/stage1-fat32.sym"
    DEPENDS stage1-fat32.elf
)
