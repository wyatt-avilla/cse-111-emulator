#include "console.h"

#include <fstream>
#include <iostream>
#include <string>

OS::OS(Console* console) : c(console) {}

void OS::reset(const std::string& filename) {
    // 1. Clear all of RAM with zeros
    this->c->memory.clearRAM();

    // Load in slug file
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::invalid_argument("couldn't open " + filename);
    }
    this->c->memory.loadFile(file);
    file.close();

    // 2. Copy data section to RAM
    this->c->memory.copyDataSectionToRam();

    // 3. Initialize stack pointer register to the end of the stack (0x3000)
    this->c->cpu.setStackPointerTo(
        static_cast<uint16_t>(Memory::Address::STACK_END)
    );

    // 4. Call setup()
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getSetupAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0) {
        uint16_t program_counter = this->c->cpu.getProgramCounter();
        uint32_t instruction = this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }
}

void OS::loopIteration() {
    // 1. Run iteration of loop()
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getLoopAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0) {
        uint16_t program_counter = this->c->cpu.getProgramCounter();
        uint32_t instruction = this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }

    // 2. The GPU frame buffer is displayed
}
