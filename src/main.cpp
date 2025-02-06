#include "memory.h"
#include "console.h"
#include "cpu.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

int32_t main(int32_t argc, char* argv[]) {
    const int32_t expected_args = 1;
    if (argc - 1 != expected_args) {
        std::cerr << "Wrong number of command line arguments. (got " << argc - 1
                  << ", expected " << expected_args << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    Console banana;

    try {
        banana.run(argv[1]);
    } catch (const std::exception& e) {
        std::cerr << "Couldn't run " << "\"" << argv[1] << "\":" << std::endl
                  << "    " << e.what() << std::endl;
    }

    exit(EXIT_SUCCESS);
}
