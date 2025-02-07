#include "console.h"

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

OS::OS(Console* c_) : c(c_) {}

void OS::reset(const std::string& filename) {
    // 1. Clear all of RAM with zeros
    c->memory.clearRAM();

    // 2. Copy data section to RAM
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::invalid_argument("couldn't open " + filename);
    }

    file.seekg(0, std::ios::beg);
    uint16_t i = 0x8000;
    char ch;
    while (file.get(ch)) {
        c->memory.w8u(i, (uint8_t) ch);
        i += 1;
    }

    // 3. Initialize stack pointer register to the end of the stack (0x3000)
    this->c->cpu.set_stack_pointer_to(0x3000);

    // 4. Call setup()
    this->c->cpu.set_program_counter_to(0xfffc);
    this->c->cpu.JAL(this->c->memory.getSetupAddress() / 4);

    while (this->c->cpu.get_program_counter() != 0) {
        uint16_t program_counter = this->c->cpu.get_program_counter();
        uint32_t instruction = this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }
}

void OS::loop() {
    // 1. Run loop()
    this->c->cpu.set_program_counter_to(0xfffc);
    this->c->cpu.JAL(this->c->memory.getLoopAddress() / 4);

    while (this->c->cpu.get_program_counter() != 0) {
        uint16_t program_counter = this->c->cpu.get_program_counter();
        uint32_t instruction = this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }

    // 2. The GPU frame buffer is displayed
}
