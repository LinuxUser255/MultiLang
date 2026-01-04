How to integrate the Rust code in this C project. Based on existing multi-language setup (C, C++), here's how to add Rust:


## Step 1: Create a Rust Library with C-Compatible Function

Create a new file for the Rust library function:

```rust
use std::io::{self, Write};
use std::ffi::CStr;
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn ask_name_rust(name: *mut c_char, size: usize) {
    print!("Enter your name (Rust version): ");
    io::stdout().flush().unwrap();

    let mut input = String::new();
    match io::stdin().read_line(&mut input) {
        Ok(_) => {
            let trimmed = input.trim();
            let bytes_to_copy = std::cmp::min(trimmed.len(), size - 1);
            
            unsafe {
                std::ptr::copy_nonoverlapping(
                    trimmed.as_ptr(),
                    name as *mut u8,
                    bytes_to_copy
                );
                *name.add(bytes_to_copy) = 0; // Null terminator
            }
            
            println!("Hello from Rust, {}!", trimmed);
        }
        Err(e) => {
            eprintln!("Error reading input: {}", e);
            unsafe {
                *name = 0; // Empty string on error
            }
        }
    }
}
```

## Step 3: Create C Header for Rust Function

```c
// File: /Users/chris/CLionProjects/MultiLang/greet_rust.h
#ifndef MULTILANG_GREET_RUST_H
#define MULTILANG_GREET_RUST_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void ask_name_rust(char *name, size_t size);

#ifdef __cplusplus
}
#endif

#endif //MULTILANG_GREET_RUST_H
```

## Step 4: Update main.c

```c
// File: /Users/chris/CLionProjects/MultiLang/main.c
#include <stdio.h>
#include <stdlib.h>
#include "get_input.h"
#include "get_input_mem.h"
#include "get_input_cpp.h"
#include "get_input_mem_cpp.h"
#include "greet_rust.h"

int main(void) {
    // ... existing code ...

    // Add Rust version
    char rust_name[100];
    ask_name_rust(rust_name, sizeof(rust_name));
    printf("Rust returned: %s\n", rust_name);

    return 0;
}
```

## Step 5: Update CMakeLists.txt

```cmake
# File: /Users/chris/CLionProjects/MultiLang/CMakeLists.txt
cmake_minimum_required(VERSION 3.28)
project(MultiLang)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Build Rust library
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    COMMAND rustc --crate-type=staticlib ${CMAKE_CURRENT_SOURCE_DIR}/greet_lib.rs -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_lib.rs
    COMMENT "Building Rust library"
)

add_custom_target(rust_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)

add_executable(MultiLang
        main.c
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
```

## Alternative: Using Cargo (Recommended)

If you prefer using Cargo for better Rust integration:

```bash
cd /Users/chris/CLionProjects/MultiLang
cargo init --lib
```

Then update `Cargo.toml`:

```toml
[package]
name = "greet_rust"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["staticlib"]

[dependencies]
```

Move `greet_lib.rs` content to `src/lib.rs` and update CMakeLists.txt:

```cmake
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    COMMAND cargo build --release
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/target/release/libgreet_rust.a ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/lib.rs
    COMMENT "Building Rust library with Cargo"
)
```
