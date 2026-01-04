# Multi-Programming Language Codebase

An educational systems programming project demonstrating how multiple programming languages can be integrated cleanly and safely within a single codebase.

---

## Overview

This project demonstrates practical, real-world techniques for building a unified application across multiple programming languages. It focuses on **foreign function interfaces (FFI)**, **ABI compatibility**, **memory ownership**, and **build system orchestration**.

Rather than treating each language in isolation, the project shows how they can interoperate through a shared C ABI, mirroring patterns used in operating systems, game engines, embedded software, and high-performance systems.

---

## Supported Languages

### Current
- **C** — Core application logic and ABI boundary
- **C++** — Object-oriented extensions and modern abstractions
- **Rust** — Memory-safe systems components via FFI

### Planned / Future
- Python (via C API, `ctypes`, or `cffi`)
- Go (via `cgo`)
- Assembly (performance-critical sections)
- JavaScript / TypeScript (Node.js native modules)
- WebAssembly targets

---

## Project Structure

```text
MultiLang/
├── src/
│   └── greet_lib.rs           # Rust static library for C interop
├── banner.c                   # C banner display logic
├── banner.h
├── get_input.c                # C stack-based input handling
├── get_input.h
├── get_input_mem.c            # C heap-based input handling
├── get_input_mem.h
├── get_input_cpp.cpp          # C++ stack-based input handling
├── get_input_cpp.h
├── get_input_mem_cpp.cpp      # C++ heap-based input handling
├── get_input_mem_cpp.h
├── greet.rs                   # Standalone Rust example
├── greet_rust.h               # C header for Rust FFI
├── main.c                     # Unified application entry point
└── CMakeLists.txt             # Multi-language build configuration
````

---

## Key Concepts Demonstrated

### 1. Foreign Function Interface (FFI)

All cross-language calls are performed through **C-compatible interfaces**, ensuring ABI stability and portability.

#### Rust → C

```rust
#[no_mangle]
pub extern "C" fn ask_name_rust(name: *mut c_char, size: usize) {
    // Rust implementation callable from C
}
```

#### C / C++ Declaration

```c
extern "C" {
    void ask_name_cpp(char *name, size_t size);
}
```

---

### 2. Memory Management Across Languages

The project intentionally contrasts multiple allocation strategies:

* **C (stack)** — Fast, automatic lifetime
* **C (heap)** — Manual `malloc` / `free`
* **C++** — C-style allocation and `std::string`
* **Rust** — Ownership-based safety without GC

Each boundary clearly documents **who allocates** and **who frees** memory.

---

### 3. Build System Integration

CMake orchestrates compilation across languages and toolchains.

```cmake
# Compile Rust static library
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a
    COMMAND rustc --crate-type=staticlib ...
)

# Link Rust into the C/C++ binary
target_link_libraries(MultiLang ${CMAKE_CURRENT_BINARY_DIR}/libgreet_rust.a)
```

This mirrors real-world polyglot build pipelines.

---

### 4. Cross-Language String Handling

The project demonstrates safe string exchange despite differing representations:

* **C:** `char *` (null-terminated)
* **C++:** `std::string` converted at boundaries
* **Rust:** UTF-8 `String` with explicit FFI conversion

---

## Building the Project

### Prerequisites

* CMake 3.28+
* C compiler (GCC or Clang)
* C++ compiler (G++ or Clang++)
* Rust toolchain (`rustc`, `cargo`)

### Build Instructions

```bash
mkdir cmake-build-debug
cd cmake-build-debug

cmake -DCMAKE_BUILD_TYPE=Debug ..
cmake --build . --target MultiLang -j 12

./MultiLang
```

---

## How It Works

### Application Flow

#### 1. Banner Display (C)

* Formatted output
* String centering
* Pure C helpers

#### 2. Input Collection

* C (stack-based)
* C (heap-based)
* C++ (stack-based)
* C++ (heap-based)
* Rust via FFI

#### 3. Cross-Language Communication

* C ABI as the common interface
* `extern "C"` and `#[no_mangle]`
* Explicit ownership rules

---

## Language Integration Patterns

### C as the Common Denominator

C serves as the interoperability layer because it provides:

* Stable ABI
* No name mangling
* Simple calling conventions
* Universal toolchain support

---

### Rust FFI Safety

```rust
// Safe Rust logic
let mut input = String::new();
io::stdin().read_line(&mut input)?;

// Unsafe boundary (explicit and minimal)
unsafe {
    std::ptr::copy_nonoverlapping(
        trimmed.as_ptr(),
        name as *mut u8,
        bytes_to_copy
    );
}
```

---

### C++ Compatibility

```cpp
void ask_name_cpp(char *name, size_t size) {
    std::string input;
    std::getline(std::cin, input);
    strncpy(name, input.c_str(), size - 1);
    name[size - 1] = '\0';
}

extern "C" {
    void ask_name_cpp(char *name, size_t size);
}
```

---

## Educational Goals

1. **Language Interoperability**

    * ABI compatibility
    * FFI design patterns

2. **Memory Management**

    * Ownership and lifetimes
    * Cross-language safety

3. **Build Systems**

    * Multi-toolchain builds
    * Static library integration

4. **Software Architecture**

    * Modular design
    * Clear separation of concerns

---

## Common Pitfalls and Solutions

### String Handling

* **Problem:** Incompatible representations
* **Solution:** Use null-terminated C strings at boundaries

### Memory Ownership

* **Problem:** Unclear allocation responsibility
* **Solution:** Document ownership; enforce consistent patterns

### Build Complexity

* **Problem:** Multiple toolchains
* **Solution:** Centralize orchestration in CMake

### Platform Differences

* **Problem:** ABI and calling convention variance
* **Solution:** Use `extern "C"` and test per platform

---

## Future Enhancements

* Python, Go, and WebAssembly integrations
* Assembly-level optimizations
* Performance benchmarks
* Expanded error-handling examples
* Platform-specific documentation

---

## Contributing

This is an educational project. Contributions that introduce new language integrations, improve safety, or enhance documentation are welcome.

### Adding a New Language

1. Create language-specific source files
2. Expose a C-compatible interface
3. Update `CMakeLists.txt`
4. Integrate into `main.c`
5. Document patterns and pitfalls

---

## License

This project is provided for educational purposes. You are free to use and modify it for learning.

---

## Resources

* Rust FFI Guide
* C++ Name Mangling
* CMake Documentation
* Foreign Function Interface (Wikipedia)

---

## Expected Output

```text
                     Multi Programming Language Codebase
                ══════════════════════════════════════════════════
                     A software development education project

=== Multi-Language Input Demo ===

--- C IMPLEMENTATIONS ---

1. C Stack-based version:
Enter your name: Alice
Hello, Alice!
Stored in C stack: Alice

2. C Heap-based version:
Enter your name: Bob
Hello, Bob!
Stored in C heap: Bob

--- C++ IMPLEMENTATIONS ---

3. C++ Stack-based version:
Enter your name: Charlie
Hello, Charlie!
Stored in C++ stack: Charlie

4. C++ Heap-based version:
Enter your name: Diana
Hello, Diana!
Stored in C++ heap: Diana

--- RUST IMPLEMENTATION ---

5. Rust version:
Enter your name (Rust version): Eve
Hello Eve From Rust!
Rust returned: Eve

=== All tests completed ===
```
