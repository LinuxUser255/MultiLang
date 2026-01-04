# Adding New Languages to MultiLang

This guide shows how to add new languages to MultiLang in a consistent, ABI-safe way. It fixes earlier mistakes and fills in missing sections. Every section is cross-linked for easy navigation.

Related docs:
- FFI & ABI overview: ./FFI-ABI.md
- size_t vs usize: ./size_t-usize.md
- End-to-end wiring example: ./tying-it-all-together.md

---

## Table of Contents

1. [Prerequisites and Layout](#prerequisites-and-layout)
2. [General Integration Pattern](#general-integration-pattern)
3. [Understanding C Types and FFI](#understanding-c-types-and-ffi)
4. [Adding Compiled Languages (C-compatible FFI)](#adding-compiled-languages)
   - [Rust](#rust-integration)
   - [Go](#go-integration)
   - [Zig](#zig-integration)
   - [D](#d-integration)
   - [Nim](#nim-integration)
5. [Adding Interpreted Languages](#adding-interpreted-languages)
   - [Python](#python-integration)
   - [Lua](#lua-integration)
   - [JavaScript (Node.js)](#javascript-integration)
6. [Adding Assembly](#assembly-integration)
7. [Adding More C/C++ Files](#adding-more-c-cpp-files)
8. [Testing Your Integration](#testing-your-integration)
9. [Troubleshooting & Pitfalls](#troubleshooting--pitfalls)

---

## Prerequisites and Layout

Assumptions (adjust for your setup):
- C or C++ is the “driver” that orchestrates calls to other languages through a C ABI.
- Build system is CMake 3.20+ (examples use add_custom_command/add_custom_target).
- New languages will expose C-callable symbols and be linked as static archives (.a) or shared libs.

Suggested layout used below:
- C/C++ headers in project root or include/
- Language sources in language-specific files under src/ or top-level

Back to top: [Table of Contents](#table-of-contents)

---

## General Integration Pattern

Use this repeatable flow for any compiled language:
1. Implement a function with a C ABI (no name mangling, simple types).
2. Declare it in a C header (extern "C").
3. Build a library artifact (.a or .so/.dylib) from that language.
4. Link the artifact into the C/C++ executable via CMake.
5. Call the function from main.c or a C/C++ module.
6. Test end-to-end.

Recommended C function shape for output buffers:
```c
int lang_func(char *buf, size_t buf_len); // returns number of bytes written (excl. NUL) or negative on error
```
If you keep a void function for simplicity, still validate inputs and NUL-terminate safely.

Back to top: [Table of Contents](#table-of-contents)

---

## Understanding C Types and FFI

- size_t is the correct type for buffer sizes and counts. See ./size_t-usize.md for a deep dive.
- Map to the language’s pointer-sized unsigned type:
  - Rust: usize
  - Zig: usize
  - Go (cgo): C.size_t (exposed by the C pseudo-package)

Rules at the boundary (summarized from ./FFI-ABI.md):
- Use extern "C" and #[no_mangle] (or language equivalent) so the symbol name is stable.
- Don’t pass language-specific strings/containers across the boundary; pass char* + length.
- Caller allocates, callee writes, and callee never frees memory allocated by the caller.
- Do not let panics/exceptions cross the boundary; return status codes instead.

Back to top: [Table of Contents](#table-of-contents)

---

## Adding Compiled Languages

### Rust Integration

Files:
- src/greet_lib.rs (or src/lib.rs if using Cargo)
- greet_rust.h

Rust (safe copying, NUL-termination, input checks):
```rust
use std::io::{self, Write};
use std::os::raw::c_char;

#[no_mangle]
pub extern "C" fn ask_name_rust(name: *mut c_char, size: usize) {
    if name.is_null() || size == 0 { return; }

    print!("Enter your name (Rust version): ");
    let _ = io::stdout().flush();

    let mut input = String::new();
    if io::stdin().read_line(&mut input).is_ok() {
        let bytes = input.trim().as_bytes();
        let n = bytes.len().min(size.saturating_sub(1));
        unsafe {
            std::ptr::copy_nonoverlapping(bytes.as_ptr(), name as *mut u8, n);
            *name.add(n) = 0;
        }
        println!("Hello from Rust, {}!", input.trim());
    } else {
        unsafe { *name = 0; }
    }
}
```

C header (greet_rust.h):
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
#endif
```

CMake (minimal, no unnecessary Apple frameworks):
```cmake
# Build a staticlib directly with rustc (or use Cargo below)
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
  COMMAND ${CMAKE_COMMAND} -E env RUSTC=rustc rustc --crate-type=staticlib
          ${CMAKE_CURRENT_SOURCE_DIR}/src/greet_lib.rs
          -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/greet_lib.rs
  COMMENT "Building Rust static library"
)
add_custom_target(rust_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)
add_dependencies(MultiLang rust_lib)
target_link_libraries(MultiLang PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)
# On Linux you may also need: pthread dl m
```

Cargo alternative (recommended for larger Rust codebases): see ./tying-it-all-together.md.

Back to top: [Table of Contents](#table-of-contents)

---

### Go Integration

File: greet_go.go (built as a c-archive)

Go (fixes previous bounds bug and size==0 handling):
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
    if name == nil || size == 0 { return }

    fmt.Print("Enter your name (Go version): ")
    input, err := bufio.NewReader(os.Stdin).ReadString('\n')
    if err != nil {
        (*(*[1 << 30]byte)(unsafe.Pointer(name)))[0] = 0
        return
    }

    trimmed := strings.TrimSpace(input)
    buf := (*[1 << 30]byte)(unsafe.Pointer(name))[:int(size):int(size)]
    l := len(buf)
    if l == 0 { return }
    maxCopy := l - 1 // leave space for NUL
    n := copy(buf[:maxCopy], []byte(trimmed))
    buf[n] = 0
}

func main() {}
```

CMake (use the header emitted by go build - the .h is generated automatically):
```cmake
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a
  COMMAND go build -buildmode=c-archive -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a
          ${CMAKE_CURRENT_SOURCE_DIR}/greet_go.go
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_go.go
  COMMENT "Building Go c-archive"
)
add_custom_target(go_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a)
add_dependencies(MultiLang go_lib)
# The c-archive also emits a matching .h next to the .a; include its directory
target_include_directories(MultiLang PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(MultiLang PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libgreet_go.a pthread)
```

Back to top: [Table of Contents](#table-of-contents)

---

### Zig Integration

File: greet_zig.zig

```zig
const std = @import("std");

export fn ask_name_zig(name: [*c]u8, size: usize) void {
    if (size == 0) return;

    const stdout = std.io.getStdOut().writer();
    const stdin = std.io.getStdIn().reader();
    _ = stdout.print("Enter your name (Zig version): ", .{});

    var buf: [256]u8 = undefined;
    const res = stdin.readUntilDelimiterOrEof(&buf, '\n') catch |err| {
        name[0] = 0; return;
    };

    if (res) |line| {
        const trimmed = std.mem.trim(u8, line, &std.ascii.whitespace);
        const n = @min(trimmed.len, size - 1);
        std.mem.copy(u8, name[0..n], trimmed[0..n]);
        name[n] = 0;
        _ = stdout.print("Hello from Zig, {s}!\n", .{trimmed});
    } else {
        name[0] = 0;
    }
}
```

CMake:
```cmake
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a
  COMMAND zig build-lib -O ReleaseSmall -static -fPIC -o ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a
          ${CMAKE_CURRENT_SOURCE_DIR}/greet_zig.zig
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_zig.zig
  COMMENT "Building Zig static library"
)
add_custom_target(zig_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a)
add_dependencies(MultiLang zig_lib)
target_link_libraries(MultiLang PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libgreet_zig.a)
```

Back to top: [Table of Contents](#table-of-contents)

---

### D Integration

File: greet_d.d

```d
module greet_d;
import core.stdc.stdint;   // size_t
import core.stdc.string : memcpy;
import std.stdio;          // readln, writef
import std.string : strip;

extern(C) export void ask_name_d(char* name, size_t size) {
    if (name is null || size == 0) return;
    writef("Enter your name (D version): ");
    auto line = readln();
    auto trimmed = line.strip();
    const len = trimmed.length;
    const n = (len < size - 1) ? len : size - 1;
    if (n > 0) memcpy(name, trimmed.ptr, n);
    name[n] = '\0';
    writef("Hello from D, %s!\n", trimmed);
}
```

CMake (using dmd/ldc2; set D_COMPILER to dmd or ldc2):
```cmake
set(D_COMPILER dmd CACHE STRING "D compiler")
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_d.a
  COMMAND ${D_COMPILER} -lib -of=${CMAKE_CURRENT_BINARY_DIR}/libgreet_d.a
          ${CMAKE_CURRENT_SOURCE_DIR}/greet_d.d
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_d.d
  COMMENT "Building D static library"
)
add_custom_target(d_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_d.a)
add_dependencies(MultiLang d_lib)
target_link_libraries(MultiLang PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libgreet_d.a)
```

Back to top: [Table of Contents](#table-of-contents)

---

### Nim Integration

File: greet_nim.nim

```nim
import strutils
import system

proc ask_name_nim(name: ptr char, size: csize) {.exportc, cdecl.} =
  if name.isNil or size == 0: return
  stdout.write("Enter your name (Nim version): ")
  let line = stdin.readLine()
  let trimmed = line.strip()
  let maxCopy = int(size) - 1
  var n = min(trimmed.len, maxCopy)
  if n > 0:
    copyMem(name, unsafeAddr trimmed[0], n)
  name[n] = '\0'
  echo "Hello from Nim, ", trimmed, "!"
```

CMake (requires Nim in PATH):
```cmake
add_custom_command(
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_nim.a
  COMMAND nim c --app:staticlib -d:release -o:${CMAKE_CURRENT_BINARY_DIR}/libgreet_nim.a
          ${CMAKE_CURRENT_SOURCE_DIR}/greet_nim.nim
  DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/greet_nim.nim
  COMMENT "Building Nim static library"
)
add_custom_target(nim_lib ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/libgreet_nim.a)
add_dependencies(MultiLang nim_lib)
target_link_libraries(MultiLang PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libgreet_nim.a)
```

Back to top: [Table of Contents](#table-of-contents)

---

## Adding Interpreted Languages

For interpreters, you have two practical choices:
1) Embed the interpreter and expose a C function (more work, tighter integration).
2) Use a subprocess bridge (simpler, great for demos and tests).

### Python Integration

Subprocess bridge example (portable):
```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int run_python(char *buf, size_t buf_len) {
  FILE *p = popen("python3 scripts/greet.py", "r");
  if (!p) return -1;
  if (fgets(buf, (int)buf_len, p) == NULL) { buf[0] = '\0'; pclose(p); return -2; }
  buf[strcspn(buf, "\n")] = '\0';
  return pclose(p);
}
```

scripts/greet.py:
```python
name = input("Enter your name (Python version): ")
print(f"Hello from Python, {name}!")
```

Embedding via the CPython C API is also possible but out of scope here.

### Lua Integration

Subprocess bridge with luajit or lua:
```c
int run_lua(char *buf, size_t buf_len) {
  FILE *p = popen("lua scripts/greet.lua", "r");
  if (!p) return -1;
  if (fgets(buf, (int)buf_len, p) == NULL) { buf[0] = '\0'; pclose(p); return -2; }
  buf[strcspn(buf, "\n")] = '\0';
  return pclose(p);
}
```

scripts/greet.lua:
```lua
io.write("Enter your name (Lua version): ")
local name = io.read("*l")
print("Hello from Lua, " .. (name or "") .. "!")
```

### JavaScript (Node.js) Integration

```c
int run_node(char *buf, size_t buf_len) {
  FILE *p = popen("node scripts/greet.js", "r");
  if (!p) return -1;
  if (fgets(buf, (int)buf_len, p) == NULL) { buf[0] = '\0'; pclose(p); return -2; }
  buf[strcspn(buf, "\n")] = '\0';
  return pclose(p);
}
```

scripts/greet.js:
```js
process.stdout.write("Enter your name (Node.js version): ");
const fs = require("node:fs");
const name = fs.readFileSync(0, "utf8").trim();
console.log(`Hello from Node, ${name}!`);
```

Back to top: [Table of Contents](#table-of-contents)

---

## Adding Assembly

Provide a C ABI symbol implemented in assembly that writes a fixed message into the buffer (keeps it simple and portable across assemblers/OSes).

Example (AT&T syntax, x86_64 System V, GAS):
```asm
.globl ask_name_asm
.type ask_name_asm, @function
ask_name_asm:
  # rdi = char *name, rsi = size_t size
  test %rsi, %rsi
  je  .ret
  lea msg(%rip), %rcx
  xor %rax, %rax
  mov %rsi, %rdx
  dec %rdx                 # leave space for NUL
  mov $0, %r8
.copy:
  cmp %r8, %rdx
  jae .nul
  movzb (%rcx,%r8,1), %al
  cmp $0, %al
  je  .nul
  mov %al, (%rdi,%r8,1)
  inc %r8
  jmp .copy
.nul:
  movb $0, (%rdi,%r8,1)
.ret:
  ret
.section .rodata
msg:
  .asciz "Hello from Assembly!"
```

Back to top: [Table of Contents](#table-of-contents)

---

## Adding More C/C++ Files

- Add headers/sources to your target in CMake:
```cmake
set(SRCS
  main.c
  get_input.c
  # add more here
)
add_executable(MultiLang ${SRCS})
```
- Put declarations in headers and include them from callers.
- Use extern "C" for C-callable functions compiled as C++.

Back to top: [Table of Contents](#table-of-contents)

---

## Testing Your Integration

Manual smoke test:
- Build the project with your new target(s) linked.
- Run the executable and exercise each language path.

Behavioral checks (all languages):
- Passing size==0 must not write.
- Output must be NUL-terminated.
- No allocation on the far side of the boundary escapes to the caller.

Back to top: [Table of Contents](#table-of-contents)

---

## Troubleshooting & Pitfalls

- Linking errors (undefined symbols): ensure #[no_mangle]/extern "C" and the library is added to target_link_libraries.
- Wrong header: for Go c-archive, include the generated header next to the .a file; don’t hand-write a mismatching one.
- macOS frameworks: not required for Rust staticlibs shown here; avoid adding random frameworks.
- Name mangling in C++: wrap declarations with extern "C" when exposing C ABI functions from C++.
- Character encodings: examples treat user input as bytes; multibyte trimming may truncate Unicode; handle as needed.

Back to top: [Table of Contents](#table-of-contents)
