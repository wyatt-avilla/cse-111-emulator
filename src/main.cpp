#include "console.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#define EXPECTED_ARGS 1
#define FILENAME_INDEX 1

int32_t main(int32_t argc, char* argv[]) {
    if (argc - 1 != EXPECTED_ARGS) {
        std::cerr << "Wrong number of command line arguments. (got " << argc - 1
                  << ", expected " << EXPECTED_ARGS << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    Console banana;

    try {
        banana.run(argv[FILENAME_INDEX]);
    } catch (const std::exception& e) {
        std::cerr << "Couldn't run " << "\"" << argv[FILENAME_INDEX]
                  << "\":" << std::endl
                  << "    " << e.what() << std::endl;
    }

    exit(EXIT_SUCCESS);
}
