#include <stdio.h>
#include <stdlib.h>
#include "banner.h"
#include "get_input.h"
#include "get_input_mem.h"
#include "get_input_cpp.h"
#include "get_input_mem_cpp.h"
#include "greet_rust.h"

// ============================================================================
// FUTURE LANGUAGE INTEGRATIONS
// ============================================================================
// Uncomment and implement as new languages are added:
//
// #include "greet_python.h"    // Python via C API or ctypes
// #include "greet_go.h"        // Go via cgo
// #include "greet_zig.h"       // Zig with C ABI
// #include "greet_nim.h"       // Nim with exportc
// #include "greet_d.h"         // D with extern(C)
// #include "greet_swift.h"     // Swift with @_cdecl
// #include "greet_kotlin.h"    // Kotlin/Native
// #include "greet_assembly.h"  // Assembly routines
// #include "greet_wasm.h"      // WebAssembly modules
// ============================================================================

int main(void) {
    // Display banner at program
    print_banner();

    printf("=== Multi-Language Input Demo ===\n\n");

    // ========== C VERSIONS ==========
    printf("--- C IMPLEMENTATIONS ---\n\n");

    // 1. C Stack-based version
    printf("1. C Stack-based version:\n");
    char name[100];
    ask_name(name, sizeof(name));
    printf("Stored in C stack: %s\n\n", name);

    // 2. C Heap-based version (malloc)
    printf("2. C Heap-based version (malloc):\n");
    char *heap_name = ask_name_malloc(100);
    if (heap_name != NULL) {
        printf("Stored in C heap: %s\n", heap_name);
        free_name(heap_name);
    }
    printf("\n");

    // ========== C++ VERSIONS ==========
    printf("--- C++ IMPLEMENTATIONS ---\n\n");

    // 3. C++ Stack-based version (called from C)
    printf("3. C++ Stack-based version:\n");
    char cpp_name[100];
    ask_name_cpp(cpp_name, sizeof(cpp_name));
    printf("Stored in C++ stack: %s\n\n", cpp_name);

    // 4. C++ Heap-based version (malloc, called from C)
    printf("4. C++ Heap-based version (malloc):\n");
    char *cpp_heap_name = ask_name_cpp_malloc(100);
    if (cpp_heap_name != NULL) {
        printf("Stored in C++ heap: %s\n", cpp_heap_name);
        free_name_cpp(cpp_heap_name);
    }
    printf("\n");

    // ========== RUST VERSION ==========
    printf("--- RUST IMPLEMENTATION ---\n\n");

    printf("5. Rust version:\n");
    char rust_name[100];
    ask_name_rust(rust_name, sizeof(rust_name));
    printf("Rust returned: %s\n\n", rust_name);

    // ========== FUTURE LANGUAGES ==========
    // Uncomment sections as new languages are integrated:

    /*
    // ========== PYTHON VERSION ==========
    printf("--- PYTHON IMPLEMENTATION ---\n\n");

    printf("6. Python version:\n");
    char python_name[100];
    ask_name_python(python_name, sizeof(python_name));
    printf("Python returned: %s\n\n", python_name);
    */

    /*
    // ========== GO VERSION ==========
    printf("--- GO IMPLEMENTATION ---\n\n");

    printf("7. Go version:\n");
    char go_name[100];
    ask_name_go(go_name, sizeof(go_name));
    printf("Go returned: %s\n\n", go_name);
    */

    /*
    // ========== ZIG VERSION ==========
    printf("--- ZIG IMPLEMENTATION ---\n\n");

    printf("8. Zig version:\n");
    char zig_name[100];
    ask_name_zig(zig_name, sizeof(zig_name));
    printf("Zig returned: %s\n\n", zig_name);
    */

    /*
    // ========== ASSEMBLY VERSION ==========
    printf("--- ASSEMBLY IMPLEMENTATION ---\n\n");

    printf("9. Assembly version:\n");
    char asm_name[100];
    ask_name_asm(asm_name, sizeof(asm_name));
    printf("Assembly returned: %s\n\n", asm_name);
    */

    /*
    // ========== ADDITIONAL LANGUAGES ==========
    // Add more language implementations here following the same pattern:
    // 1. Include the header at the top
    // 2. Add a section with clear labeling
    // 3. Call the language-specific function
    // 4. Display the result
    // 5. Handle any language-specific cleanup
    */

    printf("=== All tests completed ===\n");
    return 0;
}