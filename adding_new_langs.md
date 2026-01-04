
# Adding New Languages to the MultiLang Project

This guide provides step-by-step instructions for integrating new programming languages into the MultiLang codebase.

---

## Table of Contents

1. [General Integration Pattern](#general-integration-pattern)
2. [Adding Compiled Languages (C-Compatible FFI)](#adding-compiled-languages)
    - [Rust](#rust-integration)
    - [Go](#go-integration)
    - [Zig](#zig-integration)
    - [D](#d-integration)
    - [Nim](#nim-integration)
3. [Adding Interpreted Languages](#adding-interpreted-languages)
    - [Python](#python-integration)
    - [Lua](#lua-integration)
    - [JavaScript (Node.js)](#javascript-integration)
4. [Adding Assembly](#assembly-integration)
5. [Adding More C/C++ Files](#adding-more-c-cpp-files)
6. [Testing Your Integration](#testing-your-integration)

---

## General Integration Pattern

Every language integration follows this pattern:

1. **Create the language-specific implementation**
2. **Expose a C-compatible interface** (for compiled languages)
3. **Create a C header file** declaring the interface
4. **Update CMakeLists.txt** to build the new code
5. **Update main.c** to call the new function
6. **Test the integration**

---

## Adding Compiled Languages

### Rust Integration

#### Step 1: Create Rust Source File

**Option A: Single File (Simple)**

Create `src/greet_lib.rs`:

```rust
// File: /Users/chris/CLionProjects/MultiLang/src/greet_lib.rs
use std::io::{self, Write};
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

            println!("Hello {} From Rust!", trimmed);
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

**Option B: Cargo Project (Recommended for Complex Projects)**

#### Edit `Cargo.toml`
```toml
[package]
name = "greet_rust"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["staticlib"]

[dependencies]
```

Move code to `src/lib.rs`.

#### Step 2: Create C Header

Create `greet_rust.h`:

```c
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

#### Step 3: Update CMakeLists.txt

Add to `CMakeLists.txt`:

```cmake
# Build Rust library
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    COMMAND rustc --crate-type=staticlib ${CMAKE_CURRENT_SOURCE_DIR}/src/greet_lib.rs
            -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/greet_lib.rs
    COMMENT "Building Rust library"
)

add_custom_target(rust_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)

# Add to executable sources
add_executable(MultiLang
    # ... existing files ...
    greet_rust.h
)

add_dependencies(MultiLang rust_lib)
target_link_libraries(MultiLang ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)

# Platform-specific Rust dependencies
if(APPLE)
    target_link_libraries(MultiLang "-framework Security" "-framework Foundation")
elseif(UNIX)
    target_link_libraries(MultiLang pthread dl)
endif()
```

#### Step 4: Update main.c

Add to the includes section:

```c
#include "greet_rust.h"
```

Add to the main function:

```c
// ========== RUST VERSION ==========
printf("--- RUST IMPLEMENTATION ---\n\n");

printf("5. Rust version:\n");
char rust_name[100];
ask_name_rust(rust_name, sizeof(rust_name));
printf("Rust returned: %s\n\n", rust_name);
```

### Go Integration

#### Step 1: Create Go Source File

Create `greet_go.go`:

```go
package main

import "C"
import (
    "bufio"
    "fmt"
    "os"
    "strings"
    "unsafe"
)

//export ask_name_go
func ask_name_go(name *C.char, size C.size_t) {
    fmt.Print("Enter your name (Go version): ")

    reader := bufio.NewReader(os.Stdin)
    input, err := reader.ReadString('\n')

    if err != nil {
        fmt.Fprintf(os.Stderr, "Error reading input: %v\n", err)
        *name = 0
        return
    }

    trimmed := strings.TrimSpace(input)
    fmt.Printf("Hello from Go, %s!\n", trimmed)

    // Copy to C buffer
    goBytes := []byte(trimmed)
    maxCopy := int(size) - 1
    if len(goBytes) < maxCopy {
        maxCopy = len(goBytes)
    }

    cBytes := (*[1 << 30]byte)(unsafe.Pointer(name))[:maxCopy:maxCopy]
    copy(cBytes, goBytes)
    cBytes[maxCopy] = 0 // Null terminator
}

func main() {} // Required for cgo
```

#### Step 2: Create C Header

Create `greet_go.h`:

```c
#ifndef MULTILANG_GREET_GO_H
#define MULTILANG_GREET_GO_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void ask_name_go(char *name, size_t size);

#ifdef __cplusplus
}
#endif

#endif //MULTILANG_GREET_GO_H
```

#### Step 3: Update CMakeLists.txt

```cmake
# Build Go library
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a
    COMMAND go build -buildmode=c-archive -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a
            ${CMAKE_CURRENT_SOURCE_DIR}/greet_go.go
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_go.go
    COMMENT "Building Go library"
)

add_custom_target(go_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a)

add_dependencies(MultiLang go_lib)
target_link_libraries(MultiLang ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a)

# Go requires pthread
if(UNIX)
    target_link_libraries(MultiLang pthread)
endif()
```

#### Step 4: Update main.c

```c
#include "greet_go.h"

// In main():
printf("--- GO IMPLEMENTATION ---\n\n");
printf("7. Go version:\n");
char go_name[100];
ask_name_go(go_name, sizeof(go_name));
printf("Go returned: %s\n\n", go_name);
```

### Zig Integration

#### Step 1: Create Zig Source File

Create `greet_zig.zig`:

```zig
const std = @import("std");

export fn ask_name_zig(name: [*c]u8, size: usize) void {
    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();

    stdout.print("Enter your name (Zig version): ", .{}) catch return;

    var buffer: [256]u8 = undefined;
    const input = stdin.readUntilDelimiterOrEof(&buffer, '\n') catch {
        name[0] = 0;
        return;
    };

    if (input) |line| {
        const trimmed = std.mem.trim(u8, line, &std.ascii.whitespace);
        stdout.print("Hello from Zig, {s}!\n", .{trimmed}) catch {};

        const copy_len = @min(trimmed.len, size - 1);
        @memcpy(name[0..copy_len], trimmed[0..copy_len]);
        name[copy_len] = 0;
    } else {
        name[0] = 0;
    }
}
```

#### Step 2: Create C Header

Create `greet_zig.h`:

```c
#ifndef MULTILANG_GREET_ZIG_H
#define MULTILANG_GREET_ZIG_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void ask_name_zig(char *name, size_t size);

#ifdef __cplusplus
}
#endif

#endif //MULTILANG_GREET_ZIG_H
```

#### Step 3: Update CMakeLists.txt

```cmake
# Build Zig library
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a
    COMMAND zig build-lib ${CMAKE_CURRENT_SOURCE_DIR}/greet_zig.zig
            -target native -O ReleaseFast
    COMMAND ${CMAKE_COMMAND} -E copy
            ${CMAKE_CURRENT_SOURCE_DIR}/libgreet_zig.a
            ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_zig.zig
    COMMENT "Building Zig library"
)

add_custom_target(zig_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a)

add_dependencies(MultiLang zig_lib)
target_link_libraries(MultiLang ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a)
```

#### Step 4: Update main.c

```c
#include "greet_zig.h"

// In main():
printf("--- ZIG IMPLEMENTATION ---\n\n");
printf("8. Zig version:\n");
char zig_name[100];
ask_name_zig(zig_name, sizeof(zig_name));
printf("Zig returned: %s\n\n", zig_name);
```

### D Integration

*(Note: This section is incomplete in the original document. Add implementation details here if available, following the general pattern.)*

### Nim Integration

*(Note: This section is incomplete in the original document. Add implementation details here if available, following the general pattern.)*

## Adding Interpreted Languages

### Python Integration

*(Note: This section is incomplete in the original document. Add implementation details here if available, following the general pattern for interpreted languages.)*

### Lua Integration

*(Note: This section is incomplete in the original document. Add implementation details here if available, following the general pattern for interpreted languages.)*

### JavaScript (Node.js) Integration

*(Note: This section is incomplete in the original document. Add implementation details here if available, following the general pattern for interpreted languages.)*

## Adding Assembly

*(Note: This section is incomplete in the original document. Add implementation details here if available.)*

## Adding More C/C++ Files

*(Note: This section is incomplete in the original document. Add implementation details here if available.)*

## Testing Your Integration

*(Note: This section is incomplete in the original document. Suggested steps: Build the project using CMake, run the executable, and verify that the new language implementation prompts for input and outputs correctly.)*






