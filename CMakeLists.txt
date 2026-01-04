
cmake_minimum_required(VERSION 3.28)
project(MultiLang)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Build Rust library
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    COMMAND rustc --crate-type=staticlib ${CMAKE_CURRENT_SOURCE_DIR}/src/greet_lib.rs -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/greet_lib.rs
    COMMENT "Building Rust library"
)

add_custom_target(rust_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)

add_executable(MultiLang
        main.c
        banner.c
        banner.h
        get_input.c
        get_input.h
        get_input_mem.c
        get_input_mem.h
        get_input_cpp.cpp
        get_input_cpp.h
        get_input_mem_cpp.cpp
        get_input_mem_cpp.h
        greet_rust.h)

add_dependencies(MultiLang rust_lib)

target_link_libraries(MultiLang ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)

# Link required system libraries for Rust
if(APPLE)
    target_link_libraries(MultiLang "-framework Security" "-framework Foundation")
endif()
