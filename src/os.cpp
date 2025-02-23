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

    // Poll input from the keyboard or controller
    this->c->pollInput();

    // Write a byte to stdout for testing/debugging purposes
    // Here, we write 'A' to STDOUT, assuming the output is being captured or displayed somewhere
    this->c->memory.w8u(static_cast<uint16_t>(Memory::Address::STDOUT), 'A');

    // 1. Run iteration of loop()
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getLoopAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0 && c->isRunning()) {
        uint16_t const program_counter = this->c->cpu.getProgramCounter();
        uint32_t const instruction =
            this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }

    // Render the current frame using the GPU (which now copies from external
    // VRAM)
    this->c->gpu.renderFrame();

    auto iteration_end = std::chrono::steady_clock::now();
    double const elapsed_ms = std::chrono::duration<double, std::milli>(iteration_end - iteration_start).count();
    constexpr double target_frame_time_ms = 16.667;
    if (elapsed_ms < target_frame_time_ms) {
        std::chrono::duration<double, std::milli> const sleep_duration(
            target_frame_time_ms - elapsed_ms
        );
        std::this_thread::sleep_for(sleep_duration);
    }

    // If the stop execution address (0x7200) is written to memory, stop the OS
    // In the loop, you might want to check if the value written to 0x7200 is non-zero
    if (this->c->memory.loadByte(static_cast<uint16_t>(Memory::Address::STOP_EXECUTION)) != 0) {
        this->c->stopExecution();  // Stop the OS if the execution stop flag is set
    }

}
