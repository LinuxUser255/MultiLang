
#ifndef GET_INPUT_CPP_H
#define GET_INPUT_CPP_H

#include <stddef.h>  // for size_t

#ifdef __cplusplus
extern "C" {
#endif

// C++ version of ask_name (stack-based, C-compatible)
void ask_name_cpp(char *name, size_t size);

#ifdef __cplusplus
}

// C++-only interface using std::string
#include <string>
namespace InputCpp {
    std::string ask_name_string();
}
#endif

#endif // GET_INPUT_CPP_H
