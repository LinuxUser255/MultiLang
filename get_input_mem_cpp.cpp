// User input & memory management using C++
//
// This file demonstrates three approaches to heap-based memory management in C++:
// 1. C-compatible malloc/free (callable from C code)
// 2. Modern C++ with std::unique_ptr (automatic memory management)
// 3. Pure C++ with std::string (no manual memory management needed)

#include "get_input_mem_cpp.h"
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <limits>
#include <memory>

// ============================================================================
// C-COMPATIBLE FUNCTIONS (callable from C code via extern "C")
// ============================================================================

/**
 * @brief Allocates memory on the heap and reads user input (C-compatible)
 *
 * This function mimics the behavior of get_input_mem.c but uses C++ features
 * internally (std::getline, std::string). The interface remains C-compatible
 * so it can be called from C code.
 *
 * Memory Management:
 * - Uses malloc() for C compatibility
 * - Caller MUST free the returned pointer using free() or free_name_cpp()
 *
 * @param size Maximum size of the buffer to allocate (including null terminator)
 * @return char* Pointer to heap-allocated string, or NULL on failure
 *
 * Example usage from C:
 *   char *name = ask_name_cpp_malloc(100);
 *   if (name != NULL) {
 *       printf("Name: %s\n", name);
 *       free_name_cpp(name);  // Must free!
 *   }
 */

// Demonstrating dynamic string allocation for user input &
// Buffers I/O operations

extern "C" char* ask_name_cpp_malloc(size_t size) {
    // Step 1: Allocate memory on the heap using malloc
    // static_cast converts void* to char* (C++ requires explicit cast)
    char *name = static_cast<char*>(malloc(size * sizeof(char)));

    // Step 2: Check if allocation succeeded
    if (name == nullptr) {
        std::cerr << "Memory allocation failed (C++ version)" << std::endl;
        return nullptr;  // Return NULL pointer on failure
    }

    // Step 3: Prompt user for input
    std::cout << "Enter your name (C++ heap/malloc version): ";

    // Step 4: Safety check - ensure size doesn't exceed INT_MAX
    // (fgets in C uses int, so we maintain compatibility)
    if (size > INT_MAX) {
        size = INT_MAX;
    }

    // Step 5: Read input using C++ std::getline (more robust than fgets)
    std::string input;
    if (std::getline(std::cin, input)) {
        // Step 6: Copy input to allocated buffer
        // Use std::min to prevent buffer overflow
        size_t copy_len = std::min(input.length(), size - 1);
        std::strncpy(name, input.c_str(), copy_len);
        name[copy_len] = '\0';  // Ensure null termination

        // Step 7: Confirm input received
        std::cout << "Hello from C++ (heap), " << name << "!" << std::endl;

        // Step 8: Return pointer to caller (caller owns the memory now)
        return name;
    } else {
        // Step 9: Handle input error
        std::cerr << "Error reading input" << std::endl;
        free(name);  // Clean up allocated memory before returning
        return nullptr;
    }
}

/**
 * @brief Frees memory allocated by ask_name_cpp_malloc (C-compatible)
 *
 * This is a wrapper around free() that provides a consistent interface
 * with the C version (free_name in get_input_mem.c).
 *
 * Why this wrapper exists:
 * - Provides symmetry with ask_name_cpp_malloc()
 * - Allows for future enhancements (e.g., logging, debugging)
 * - Makes the API more explicit about ownership
 *
 * @param name Pointer to heap-allocated string (can be NULL)
 *
 * Example usage:
 *   char *name = ask_name_cpp_malloc(100);
 *   // ... use name ...
 *   free_name_cpp(name);  // Safe even if name is NULL
 */
extern "C" void free_name_cpp(char *name) {
    // Check for NULL pointer before freeing (defensive programming)
    if (name != nullptr) {
        free(name);  // Return memory to the heap
        // Note: After this call, 'name' pointer in caller is now dangling
        // Caller should set it to NULL after calling this function
    }
}

// ============================================================================
// PURE C++ FUNCTIONS (NOT callable from C code)
// ============================================================================

namespace InputCppMem {

    /**
     * @brief Allocates memory using std::unique_ptr (modern C++ approach)
     *
     * This function demonstrates RAII (Resource Acquisition Is Initialization).
     * The returned unique_ptr automatically manages memory - no manual free needed!
     *
     * Advantages over malloc:
     * - Automatic cleanup (destructor frees memory)
     * - Exception-safe (memory freed even if exception thrown)
     * - Move semantics (ownership transfer is explicit)
     * - No memory leaks possible
     *
     * @param size Maximum size of the buffer to allocate
     * @return std::unique_ptr<char[]> Smart pointer owning the allocated memory
     *
     * Example usage (C++ only):
     *   auto name = InputCppMem::ask_name_unique(100);
     *   if (name) {
     *       std::cout << "Name: " << name.get() << std::endl;
     *   }
     *   // Memory automatically freed when 'name' goes out of scope!
     */
    std::unique_ptr<char[]> ask_name_unique(size_t size) {
        std::cout << "Enter your name (C++ unique_ptr version): ";

        // Allocate memory using std::make_unique (C++14 feature)
        // This is safer than 'new' because it's exception-safe
        auto name = std::make_unique<char[]>(size);

        // Read input
        std::string input;
        if (std::getline(std::cin, input)) {
            // Copy to buffer
            size_t copy_len = std::min(input.length(), size - 1);
            std::strncpy(name.get(), input.c_str(), copy_len);
            name[copy_len] = '\0';

            std::cout << "Hello from C++ (unique_ptr), " << name.get() << "!" << std::endl;

            // Return ownership of the unique_ptr to caller
            // Memory will be freed when the returned unique_ptr is destroyed
            return name;
        }

        std::cerr << "Error reading input" << std::endl;
        return nullptr;  // Return empty unique_ptr on error
        // Memory automatically freed here if allocation succeeded
    }

    /**
     * @brief Reads user input into a std::string (best C++ approach)
     *
     * This is the most idiomatic C++ approach - no manual memory management at all!
     * std::string handles all memory allocation and deallocation automatically.
     *
     * Why this is the best approach in pure C++:
     * - No manual memory management
     * - Automatic resizing (no buffer overflow possible)
     * - Exception-safe
     * - Works with all STL algorithms and containers
     * - No performance penalty (move semantics)
     *
     * @return std::string The user's input (empty string on error)
     *
     * Example usage (C++ only):
     *   std::string name = InputCppMem::ask_name_managed();
     *   if (!name.empty()) {
     *       std::cout << "Name: " << name << std::endl;
     *   }
     *   // No cleanup needed - std::string destructor handles everything!
     */
    std::string ask_name_managed() {
        std::cout << "Enter your name (C++ managed string version): ";

        // std::string automatically manages its own memory
        // No need to specify size - it grows as needed
        std::string name;

        if (std::getline(std::cin, name)) {
            std::cout << "Hello from C++ (managed), " << name << "!" << std::endl;

            // Return by value - C++11 move semantics make this efficient
            // No copying occurs, ownership is transferred
            return name;
        }

        std::cerr << "Error reading input" << std::endl;
        return "";  // Return empty string on error
        // No cleanup needed - std::string destructor called automatically
    }
}

// ============================================================================
// MEMORY MANAGEMENT COMPARISON
// ============================================================================
//
// Function                      | Memory Type | Cleanup Required | C-Compatible
// ------------------------------|-------------|------------------|-------------
// ask_name_cpp_malloc()         | Heap (malloc)| YES (free_name_cpp) | YES
// ask_name_unique()             | Heap (new)  | NO (automatic)   | NO
// ask_name_managed()            | Heap (auto) | NO (automatic)   | NO
//
// ============================================================================
// WHEN TO USE EACH APPROACH
// ============================================================================
//
// 1. ask_name_cpp_malloc() - Use when:
//    - You need to call from C code
//    - You need C-compatible ABI
//    - You're interfacing with C libraries
//
// 2. ask_name_unique() - Use when:
//    - You need a C-style buffer in C++ code
//    - You want automatic cleanup
//    - You need to pass ownership between functions
//
// 3. ask_name_managed() - Use when:
//    - You're writing pure C++ code
//    - You want the simplest, safest approach
//    - You don't need a C-style buffer
//
// ============================================================================