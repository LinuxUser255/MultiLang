#include "get_input.h"
#include <stdio.h>
#include <string.h>
#include <limits.h>

// user input is stored on the stack, (not heap), so it can be accessed directly
void ask_name(char *name, size_t size) {
    printf("Enter your name: ");
    if (size > INT_MAX) {
        size = INT_MAX;
    }
    if (fgets(name, (int) size, stdin) != NULL) {
        name[strcspn(name, "\n")] = '\0'; // remove newline character
        printf("Hello, %s!\n", name);
    }
}