#  `size_t` in C Programming

<br>

## Explanation of `size_t`

```c

/*
 * size_t EXPLANATION
 * ==================
 * 
 * size_t is an unsigned integer type defined in <stddef.h> (and other headers
 * like <stdio.h>, <stdlib.h>, <string.h>).
 * 
 * KEY CHARACTERISTICS:
 * 
 * 1. UNSIGNED: Can only represent non-negative values (0 and positive)
 *    - Range: 0 to SIZE_MAX (platform-dependent maximum)
 * 
 * 2. PLATFORM-DEPENDENT SIZE:
 *    - 32-bit systems: typically 4 bytes (0 to 4,294,967,295)
 *    - 64-bit systems: typically 8 bytes (0 to 18,446,744,073,709,551,615)
 * 
 * 3. GUARANTEED TO HOLD ANY ARRAY INDEX OR OBJECT SIZE
 *    - Large enough to represent the size of the largest possible object
 *    - Used for array indexing and memory sizes
 * 
 * WHY USE size_t?
 * 
 * 1. PORTABILITY: Automatically adjusts to platform architecture
 * 2. CORRECTNESS: Matches return types of sizeof operator
 * 3. SAFETY: Unsigned type prevents negative size errors
 * 4. STANDARD LIBRARY COMPATIBILITY: Used throughout C standard library
 * 
 * COMMON USES:
 * 
 * - sizeof operator returns size_t
 * - malloc/calloc/realloc take size_t parameters
 * - strlen returns size_t
 * - Array indices and loop counters for large arrays
 * - Function parameters representing sizes or counts
 * 
 * EXAMPLE COMPARISONS:
 * 
 * int size = 100;           // Signed, fixed size (usually 4 bytes)
 *                           // Can be negative (error-prone for sizes)
 *                           // May not hold large array sizes on 64-bit
 * 
 * size_t size = 100;        // Unsigned, platform-appropriate size
 *                           // Cannot be negative (safer for sizes)
 *                           // Guaranteed to hold any valid size
 * 
 * PRINTF FORMAT SPECIFIER:
 * 
 * printf("%zu", size);      // Correct way to print size_t
 * printf("%lu", (unsigned long)size);  // Alternative with cast
 */

#include "get_input_mem.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

// Demonstrating dynamic string allocation for user input &
// Buffers for I/O Operations

/**
 * Allocates memory for a name string and reads user input
 * 
 * @param size Maximum number of characters to read (including null terminator)
 *             Using size_t ensures we can handle any valid buffer size
 * @return Pointer to allocated string, or NULL on failure
 */
char* ask_name_malloc(size_t size) {
    // Allocates `size` bytes of memory on the heap for the name
    // malloc takes size_t parameter - perfect match!
    char *name = malloc(size * sizeof(char));
    if (name == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }

    printf("Enter your name: ");
    
    // fgets expects int, but size_t might be larger on 64-bit systems
    // We need to check and safely convert
    if (size > INT_MAX) {
        size = INT_MAX;
    }

    // Read user input and store it in the allocated memory
    if (fgets(name, (int) size, stdin) != NULL) {
        // strcspn returns size_t - another perfect match!
        name[strcspn(name, "\n")] = '\0'; // remove newline character
        printf("Hello, %s!\n", name);
        return name;
    } else {
        // free the allocated memory
        free(name);
        return NULL;
    }
}

// Wrapper Function to deallocate/free Memory - encapsulates memory management
void free_name(char *name) {
    if (name != NULL) {
        free(name);
    }
}
```
<br>

## Quick Reference
```c
// Common size_t usage patterns:

// 1. With sizeof
size_t array_size = sizeof(array) / sizeof(array[0]);

// 2. With malloc
char *buffer = malloc(100 * sizeof(char));  // malloc takes size_t

// 3. With strlen
size_t length = strlen("Hello");  // strlen returns size_t

// 4. Loop counters (for large arrays)
for (size_t i = 0; i < array_length; i++) {
    // process array[i]
}

// 5. Function parameters for sizes
void process_buffer(char *buffer, size_t buffer_size);
```
#### Key Takeaway:

Use size_t whenever you're dealing with sizes, counts, or array indices. It's the "correct" type for these purposes in
C.

<br>


## `size_t` and Rust's Unsigned ints

`size_t` is very similar to Rust's unsigned integer types. 
Here's a detailed comparison:

```c

/*
 * size_t vs Rust's Unsigned Integer Types
 * ========================================
 * 
 * C's size_t is VERY similar to Rust's unsigned integer types!
 * 
 * RUST UNSIGNED TYPES:
 * --------------------
 * u8   - 8-bit unsigned  (0 to 255)
 * u16  - 16-bit unsigned (0 to 65,535)
 * u32  - 32-bit unsigned (0 to 4,294,967,295)
 * u64  - 64-bit unsigned (0 to 18,446,744,073,709,551,615)
 * u128 - 128-bit unsigned
 * usize - Platform-dependent unsigned (like C's size_t!)
 * 
 * C UNSIGNED TYPES:
 * -----------------
 * unsigned char      - typically 8-bit  (like Rust's u8)
 * unsigned short     - typically 16-bit (like Rust's u16)
 * unsigned int       - typically 32-bit (like Rust's u32)
 * unsigned long      - 32-bit or 64-bit depending on platform
 * unsigned long long - typically 64-bit (like Rust's u64)
 * size_t            - Platform-dependent (like Rust's usize!)
 * 
 * THE KEY SIMILARITY:
 * -------------------
 * C's size_t ≈ Rust's usize
 * 
 * Both are:
 * - Unsigned (no negative values)
 * - Platform-dependent size (32-bit on 32-bit systems, 64-bit on 64-bit)
 * - Used for sizes, lengths, and array indexing
 * - Guaranteed to hold any valid array index or object size
 * 
 * COMPARISON EXAMPLES:
 * --------------------
 * 
 * Rust:
 *   let size: usize = 100;
 *   let buffer = vec![0u8; size];
 *   let length: usize = buffer.len();
 * 
 * C:
 *   size_t size = 100;
 *   char *buffer = malloc(size);
 *   size_t length = strlen(buffer);
 * 
 * WHY PLATFORM-DEPENDENT?
 * -----------------------
 * On 32-bit systems:
 *   - size_t is 32 bits (4 bytes) - can address up to 4GB
 *   - usize is 32 bits (4 bytes)
 * 
 * On 64-bit systems:
 *   - size_t is 64 bits (8 bytes) - can address up to 16 exabytes
 *   - usize is 64 bits (8 bytes)
 * 
 * This matches the pointer size on the platform!
 * 
 * FIXED-SIZE ALTERNATIVES IN C:
 * ------------------------------
 * C99 introduced <stdint.h> with fixed-size types (more like Rust):
 * 
 * uint8_t  - exactly 8 bits  (like Rust's u8)
 * uint16_t - exactly 16 bits (like Rust's u16)
 * uint32_t - exactly 32 bits (like Rust's u32)
 * uint64_t - exactly 64 bits (like Rust's u64)
 * 
 * But for sizes and indexing, still use size_t/usize!
 */

#include "get_input_mem.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>  // For fixed-size types like uint32_t

// Demonstrating dynamic string allocation for user input &
// Buffers for I/O Operations

/**
 * Example showing size_t (like Rust's usize)
 * 
 * In Rust, this would be:
 * fn ask_name_malloc(size: usize) -> Option<String>
 */
char* ask_name_malloc(size_t size) {
    // size_t here is like Rust's usize - platform-appropriate unsigned integer
    
    // Allocates `size` bytes of memory on the heap for the name
    char *name = malloc(size * sizeof(char));
    if (name == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }

    printf("Enter your name: ");
    
    // fgets expects int, but size_t might be larger on 64-bit systems
    // We need to check and safely convert
    if (size > INT_MAX) {
        size = INT_MAX;
    }

    // Read user input and store it in the allocated memory
    if (fgets(name, (int) size, stdin) != NULL) {
        // strcspn returns size_t (like Rust's usize)
        name[strcspn(name, "\n")] = '\0'; // remove newline character
        printf("Hello, %s!\n", name);
        return name;
    } else {
        // free the allocated memory
        free(name);
        return NULL;
    }
}

// Wrapper Function to deallocate/free Memory - encapsulates memory management
void free_name(char *name) {
    if (name != NULL) {
        free(name);
    }
}

/*
 * QUICK REFERENCE TABLE:
 * ======================
 * 
 * Purpose              | C Type    | Rust Type | Size
 * ---------------------|-----------|-----------|------------------
 * 8-bit unsigned       | uint8_t   | u8        | 1 byte
 * 16-bit unsigned      | uint16_t  | u16       | 2 bytes
 * 32-bit unsigned      | uint32_t  | u32       | 4 bytes
 * 64-bit unsigned      | uint64_t  | u64       | 8 bytes
 * Platform size/index  | size_t    | usize     | 4 or 8 bytes
 * 
 * WHEN TO USE WHAT:
 * -----------------
 * - Use size_t/usize for: array sizes, lengths, indices, memory sizes
 * - Use uint32_t/u32 for: fixed-size data, network protocols, file formats
 * - Use uint64_t/u64 for: large counters, timestamps, IDs
 */
```

<br>

**TL;DR**:
- `size_t` in C ≈ `usize` in Rust (platform-dependent unsigned)
- C also has `uint32_t`, `uint64_t` etc. (like Rust's `u32`, `u64`)
- Use `size_t`/`usize` for sizes and indices
- Use fixed-size types when you need exact bit
  
<br>
