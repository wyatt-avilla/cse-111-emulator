#include "console.h"
#include "cpu.h"
#include "banana_memory.h"

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>


void test_for_mem () {
    // goin to try to read certain memory locations so that we can test if the memory works
    Console* mytestconsole = new Console("/home/mike/Documents/college/cse111/cse-111-emulator/hws");
    std::string filename = "/home/mike/Documents/college/cse111/cse-111-emulator/hws/hello_world1.slug";
    BananaMemory* memory = new BananaMemory(mytestconsole, filename);

    // reading a single byte
    memory->w8u(0x7110, 25);
    uint8_t res1 = memory->l8u(0x7110);
    std::cout << "reading cell 0x7110 : " <<  static_cast<int>(res1)<< "; should be 25" << std::endl;

    // reading and writing two bytes
    memory->w16u(0x7120, 6500);
    uint16_t res2 = memory->l16u(0x7120);
    std::cout << "reading cell 0x7120 : " <<  static_cast<int>(res2)<< "; should be 6500" << std::endl;

    // reading and writing instruciton
    memory->writeInstrcution(0x7130, 1006500);
    uint32_t res3 = memory->loadInstruction(0x7130);
    std::cout << "reading cell 0x7120 : " <<  static_cast<int>(res3)<< "; should be 1006500" << std::endl;


    //std::cout << "test not yet done working on it" << std::endl;

    // Don't forget to free memory if dynamically allocated
    delete memory;
    delete mytestconsole;
}

int32_t main(int32_t argc, char* argv[]) {
    test_for_mem();
    return 0; // used for testing reasons
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
