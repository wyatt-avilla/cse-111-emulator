#include "console.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>


int32_t main(int32_t argc, char* argv[]) {
    const int32_t expected_args = 1;
    if (argc - 1 != expected_args) {
        std::cerr << "Wrong number of command line arguments. (got " << argc - 1
                  << ", expected" << expected_args << ")" << std::endl;
    }

    try {
        Console banana(argv[1]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Couldn't initialize emulator {" << e.what() << "}"
                  << std::endl;
    }

    exit(EXIT_SUCCESS);
}
