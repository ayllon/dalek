cmake_minimum_required (VERSION 2.8)

add_definitions (
    "-ffreestanding -fno-stack-protector -fno-builtin -nostdinc -m32"
)

add_executable (stage1.elf stage1.s)
set_target_properties (stage1.elf
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -m elf_i386"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

add_custom_target (stage1.bin
    COMMAND "${OBJCOPY}" -S -O binary "${CMAKE_BINARY_DIR}/stage1.elf" "${CMAKE_BINARY_DIR}/stage1.bin"
    COMMAND "${OBJCOPY}" --only-keep-debug "${CMAKE_BINARY_DIR}/stage1.elf" "${CMAKE_BINARY_DIR}/stage1.sym"
    DEPENDS stage1.elf
)
