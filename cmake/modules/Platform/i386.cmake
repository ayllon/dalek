find_program (GCC
    NAMES i386-elf-gcc gcc
)
find_program (LD
    NAMES i386-elf-gcc ld
)
find_program (OBJCOPY
    NAMES i386-elf-objcopy objcopy
)

set (CMAKE_C_COMPILER "${GCC}")
set (CMAKE_LINKER     "${LD}")

set (CMAKE_ASM_LINK_EXECUTABLE "${CMAKE_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET>")
set (CMAKE_C_LINK_EXECUTABLE   "${CMAKE_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET>")
set (CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_LINKER} <LINK_FLAGS> <OBJECTS> -o <TARGET>")
