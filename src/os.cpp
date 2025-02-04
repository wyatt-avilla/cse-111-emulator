#include "os.h"

#include <string>
#include <iostream>
#include <fstream>

OS::OS(Console* c_) {
    c = c_;
}

void OS::reset(std::string& filename) {
    // 1. Clear all of RAM with zeros
    for (uint16_t i = 0; i < 0x7000; i += 16) {
        // m->w16u(i, 0);
    }

    // 2. Copy data section to RAM
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    file.seekg(0, std::ios::beg);
    uint16_t i = 0x8000;
    char ch;
    while (file.get(ch)) {
        c->memory->w8u(i, (uint8_t) ch);
        i += 8;
    }

    // 3. Initialize stack pointer register to the end of the stack (0x3000)
    // cpu->L16(); // TODO implement memory/cpu into console

    // 4. Call setup()


    // 5. Begin Game Loop Sequence
    loop();
}

void OS::loop() {
    // 1. Run loop()


    // 2. The GPU frame buffer is displayed
    
}