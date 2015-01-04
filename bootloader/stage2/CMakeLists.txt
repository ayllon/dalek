cmake_minimum_required (VERSION 2.8)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

file (GLOB stage2_src  "*.S" "*.c")
file (GLOB keymaps_src "keymaps/*.c")
file (GLOB cmds_src    "commands/*.c")
file (GLOB dev_src     "devices/*/*.c")

add_definitions (
    "-ffreestanding -fno-stack-protector -fno-builtin -nostdinc -Wall -m32"
)

add_executable (stage2.elf
    ${stage2_src}
    ${keymaps_src}
    ${cmds_src}
    ${dev_src}
)
set_target_properties (stage2.elf
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld -Map ${CMAKE_BINARY_DIR}/root/stage2.map"
)
set_source_files_properties(
    main.c PROPERTIES OBJECT_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)

# Extract raw binary and debug symbols
add_custom_command (OUTPUT "${CMAKE_BINARY_DIR}/root"
    COMMAND mkdir -p "${CMAKE_BINARY_DIR}/root"
)

add_custom_target (stage2.bin
    COMMAND "${OBJCOPY}" -S -O binary "stage2.elf" "${CMAKE_BINARY_DIR}/root/stage2.bin"
    COMMAND "${OBJCOPY}" --only-keep-debug "stage2.elf" "${CMAKE_BINARY_DIR}/root/stage2.sym"
    DEPENDS stage2.elf "${CMAKE_BINARY_DIR}/root"
)
