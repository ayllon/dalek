if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (CMAKE_C_COMPILER   "i386-elf-gcc")
    set (CMAKE_C_LINKER     "i386-elf-ld")
else ()
    set (CMAKE_C_COMPILER   "gcc")
    set (CMAKE_C_LINKER     "ld")
endif ()

set (CMAKE_C_FLAGS   "-ffreestanding -fno-stack-protector -fno-builtin -nostdinc -Wall -g0 -m32" CACHE STRING "" FORCE)
set (CMAKE_ASM_FLAGS "${CMAKE_C_FLAGS}" CACHE STRING "" FORCE)

set (CMAKE_EXE_LINKER_FLAGS "-m elf_i386" CACHE STRING "" FORCE)

set (CMAKE_ASM_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET>")
set (CMAKE_C_LINK_EXECUTABLE   "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET>")
set (CMAKE_CXX_LINK_EXECUTABLE "<CMAKE_LINKER> <LINK_FLAGS> <OBJECTS> -o <TARGET>")
