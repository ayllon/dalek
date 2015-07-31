if(CMAKE_USER_MAKE_RULES_OVERRIDE)
  # Save the full path of the file so try_compile can use it.
  include(${CMAKE_USER_MAKE_RULES_OVERRIDE} RESULT_VARIABLE _override)
  set(CMAKE_USER_MAKE_RULES_OVERRIDE "${_override}")
endif()

if(CMAKE_USER_MAKE_RULES_OVERRIDE_Rust)
  # Save the full path of the file so try_compile can use it.
   include(${CMAKE_USER_MAKE_RULES_OVERRIDE_Rust} RESULT_VARIABLE _override)
   set(CMAKE_USER_MAKE_RULES_OVERRIDE_Rust "${_override}")
endif()

# compile a Rust file into an object file
if(NOT CMAKE_Rust_COMPILE_OBJECT)
  set(CMAKE_Rust_COMPILE_OBJECT
    "<CMAKE_Rust_COMPILER> -o <OBJECT> --crate-type=lib --emit=obj -C no-stack-check <SOURCE>")
endif()

if(WIN32 AND NOT CYGWIN)
  set(CMAKE_INCLUDE_FLAG_SEP_Rust ";")
else()
  set(CMAKE_INCLUDE_FLAG_SEP_Rust ":")
endif()
