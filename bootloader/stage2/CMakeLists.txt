cmake_minimum_required (VERSION 2.8)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

file (GLOB source_asm "*.S")
file (GLOB source_c   "*.c" "commands/*.c" "keymaps/*.c")

add_definitions (
    "-ffreestanding -fno-stack-protector -fno-builtin -nostdinc -Wall -m32 -O2"
)

add_executable (stage2.elf ${source_asm} ${source_c})
set_target_properties (stage2.elf
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

# Extract raw binary and debug symbols
add_custom_command (OUTPUT "${CMAKE_BINARY_DIR}/root"
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/root"
)

add_custom_target (stage2.bin
    COMMAND "${OBJCOPY}" -O binary "stage2.elf" "${CMAKE_BINARY_DIR}/root/stage2.bin"
    COMMAND "${OBJCOPY}" --only-keep-debug "stage2.elf" "${CMAKE_BINARY_DIR}/root/stage2.sym"
    DEPENDS stage2.elf "${CMAKE_BINARY_DIR}/root"
)
