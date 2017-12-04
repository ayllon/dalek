cmake_minimum_required (VERSION 2.8)

add_definitions (
    "-ffreestanding -fno-stack-protector -fno-builtin -nostdinc"
)
set (CMAKE_EXE_LINKER_FLAGS
    "-n -nostartfiles -nodefaultlibs -nostdlib -no-pie"
)

add_executable (kernel.elf
    boot.s
    long_mode.s
    multiboot.s
)

set_target_properties (kernel.elf
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
)
