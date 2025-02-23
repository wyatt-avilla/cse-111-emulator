#include "console.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

const uint8_t EXPECTED_ARGS = 1;
const uint8_t FILENAME_INDEX = 1;

int32_t main(const int32_t argc, const char* argv[]) {
    if (argc - 1 != EXPECTED_ARGS) {
        std::cerr << "Wrong number of command line arguments. (got " << argc - 1
                  << ", expected " << EXPECTED_ARGS << ")" << std::endl;
        exit(EXIT_FAILURE);
    }

    Console banana;

    try {
       // Initialize the OS
       OS os(&banana);

       // Reset the OS with the file from command line arguments
       os.reset(argv[FILENAME_INDEX]);

       // Main loop
       bool running = true;
       while (running) {
           // Poll for input (keyboard/controller)
           banana.pollInput(); 

           // Perform one iteration of the OS loop
           os.loopIteration(); 
       }
   } catch (const std::exception& e) {
       std::cerr << "Couldn't run " << "\"" << argv[FILENAME_INDEX]
                 << "\":" << std::endl
                 << "    " << e.what() << std::endl;
   }

   exit(EXIT_SUCCESS);
}