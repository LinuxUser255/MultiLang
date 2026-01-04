
#ifndef GET_INPUT_MEM_CPP_H
#define GET_INPUT_MEM_CPP_H

#include <stddef.h>  // for size_t

#ifdef __cplusplus
extern "C" {
#endif

// C++ version of ask_name_malloc (heap-based, C-compatible)
char* ask_name_cpp_malloc(size_t size);

// C++ version of free_name (C-compatible)
void free_name_cpp(char *name);

#ifdef __cplusplus
}

// C++-only interface using std::unique_ptr and std::string
#include <memory>
#include <string>
namespace InputCppMem {
    std::unique_ptr<char[]> ask_name_unique(size_t size);
    std::string ask_name_managed();
}
#endif

#endif // GET_INPUT_MEM_CPP_H
