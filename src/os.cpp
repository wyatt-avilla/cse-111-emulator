#include "os.h"

#include "console.h"
#include "gpu.h"
#include "memory.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

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
    setup();
}
void OS::setup() {
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getSetupAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0 && c->isRunning()) {
        uint16_t const program_counter = this->c->cpu.getProgramCounter();
        uint32_t const instruction =
            this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }
}
void OS::loopIteration() {
    auto iteration_start = std::chrono::steady_clock::now();
    std::cerr << " OS Loop Iteration Started" << std::endl;

    // Poll input from the keyboard/controller
    this->c->pollInput();

    // Debug: Check if something is writing to 0x7000
    std::cerr << " Checking Memory Address 0x7000: " 
              << static_cast<int>(this->c->memory.loadByte(0x7000)) << std::endl;

    // 1. Run iteration of loop()
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getLoopAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0 && c->isRunning()) {
        uint16_t program_counter = this->c->cpu.getProgramCounter();
        uint32_t instruction = this->c->memory.loadInstruction(program_counter);

        std::cerr << " Executing Instruction: " << std::hex << instruction 
                  << " at PC: " << program_counter << std::endl;

        this->c->cpu.execute(instruction);
    }

    // Render the frame
    this->c->gpu.renderFrame();

    // Debugging: Ensure 0x7000 is not being written
    std::cerr << " Checking Memory Address 0x7000 After Execution: " 
              << static_cast<int>(this->c->memory.loadByte(0x7000)) << std::endl;

    // If stop execution flag is set, stop the emulator
    if (this->c->memory.loadByte(static_cast<uint16_t>(Memory::Address::STOP_EXECUTION)) != 0) {
        std::cerr << " Stop execution flag detected!" << std::endl;
        this->c->stopExecution();
    }
}