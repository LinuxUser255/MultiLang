#ifndef MULTILANG_GET_INPUT_H
#define MULTILANG_GET_INPUT_H

#include <stddef.h>

// declare functions from other files in this header file
// avoid buffer overflows by specifying the size of the name buffer
void ask_name(char *name, size_t size);

#endif //MULTILANG_GET_INPUT_H
