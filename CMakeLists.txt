cmake_minimum_required (VERSION 2.8)
project (dalek C ASM)

set (CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}/cmake/modules/")
set (CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}")

add_subdirectory (bootloader)

include (MakeFloppy REQUIRED)
