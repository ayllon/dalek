cmake_minimum_required (VERSION 2.8)

include_directories ("${CMAKE_CURRENT_SOURCE_DIR}/include")

file (GLOB source_asm "*.S")
file (GLOB source_c   "*.c" "commands/*.c" "keymaps/*.c")

add_executable (stage2.bin ${source_asm} ${source_c})
set_target_properties (stage2.bin PROPERTIES LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld")
