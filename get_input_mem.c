#include "get_input_mem.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

// Demonstrating dynamic string allocation for user input &
// Buffers for I/O Operations


char* ask_name_malloc(size_t size) {
    // Allocates 100 bytes`size` bytes of memory on the heap for the name
    char *name = malloc(size * sizeof(char));
    if (name == NULL) {
        fprintf(stderr, "Failed to allocate memory\n");
        return NULL;
    }

    printf("Enter your name: ");
    if (size > INT_MAX) {
        size = INT_MAX;
    }

    // Read user input and store it in the allocated memory
    if (fgets(name, (int) size, stdin) != NULL) {
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

