// C++ implementation of asking for user input using std::string
#include "get_input_cpp.h"
#include <iostream>
#include <string>
#include <cstring>

// C-compatible function (stack-based, similar to get_input.c)
extern "C" void ask_name_cpp(char *name, size_t size) {
    std::cout << "Enter your name (C++ version): ";

    // check size limmit
    if (size > INT_MAX) {
        size = INT_MAX;
    }
    std::string input;
    if (std::getline(std::cin, input)) {
        // copy to buffer, ensuring null termination
        size_t copy_len = std::min(input.length(), size - 1);
        std::strncpy(name, input.c_str(), copy_len);
        name[copy_len] = '\0';

        std::cout << "Hello from C++,  " << name << "!" << std::endl;
    } else {
        name[0] = '\0'; // Empty string on error
        std::cerr << "Error: Failed to read input." << std::endl;
    }
}

// Pure C++ version using std::string (not callable from C)
//namespace InputCpp {
//    std::string ask_name_string() {
//        std::cout << "Enter your name (C++ std::string version): ";
//
//        std::string name;
//        if (std::getline(std::cin, name)) {
//            std::cout << "Hello from C++, " << name << "!" << std::endl;
//            return name;
//        }
//
//        std::cerr << "Error reading input" << std::endl;
//        return "";
//    }
//}
//
















