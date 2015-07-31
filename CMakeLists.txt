cmake_minimum_required (VERSION 2.8)

set (CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}/cmake/modules/")
project (dalek C ASM Rust)


add_subdirectory (bootloader)

set (CMAKE_MODULE_PATH  "${CMAKE_SOURCE_DIR}")
include (MakeFloppy REQUIRED)
