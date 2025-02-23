#include "console.h"
#include "os.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

const uint8_t EXPECTED_ARGS = 1;
const uint8_t FILENAME_INDEX = 1;

int32_t main(const int32_t argc, const char* argv[]) {
    if (argc - 1 != 1) {
        std::cerr << "Wrong number of command line arguments." << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cerr << " Initializing Banana Emulator..." << std::endl;
    Console banana;

    try {
        std::cerr << " Starting OS reset..." << std::endl;
        OS os(&banana);

        std::cerr << " Loading SLUG file: " << argv[1] << std::endl;
        os.reset(argv[1]);  // 🚨 Error likely happens here!

        std::cerr << " Entering main loop..." << std::endl;
        while (banana.isRunning()) {
            banana.pollInput();
            os.loopIteration();
        }
    } catch (const std::exception& e) {
        std::cerr << " Couldn't run " << "\"" << argv[1] << "\":" << std::endl
                  << "    " << e.what() << std::endl;
    }

    exit(EXIT_SUCCESS);
}