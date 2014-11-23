cmake_minimum_required (VERSION 2.8)

add_executable (stage1.bin stage1.s)
set_target_properties (stage1.bin
    PROPERTIES
    LINK_FLAGS "-T ${CMAKE_CURRENT_SOURCE_DIR}/linker.ld"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

