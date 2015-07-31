find_program (CMAKE_Rust_COMPILER
    rustc
)
set(CMAKE_Rust_COMPILER_ENV_VAR "RUST_COMPILER")
configure_file(${CMAKE_MODULE_PATH}/CMakeRustCompiler.cmake.in
  ${CMAKE_PLATFORM_INFO_DIR}/CMakeRustCompiler.cmake @ONLY)
