#include "console.h"

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>

OS::OS(Console* console) : c(console) {}

void OS::reset(const std::string& filename) {
    this->c->memory.clearRAM();

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::invalid_argument("couldn't open " + filename);
    }
    this->c->memory.loadFile(file);
    file.close();

    this->c->memory.copyDataSectionToRam();

    this->c->cpu.setStackPointerTo(static_cast<uint16_t>(Memory::Address::STACK_END));

    setup();
}

void OS::setup() {
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getSetupAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0 && c->isRunning()) {
        uint16_t const program_counter = this->c->cpu.getProgramCounter();
        uint32_t const instruction = this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }
}

void OS::loopIteration() {
    // Start timing the iteration
    auto iterationStart = std::chrono::steady_clock::now();

    // 1. Run iteration of loop()
    this->c->cpu.setProgramCounterTo(PC_RESET_VAL);
    this->c->cpu.JAL(this->c->memory.getLoopAddress() / 4);

    while (this->c->cpu.getProgramCounter() != 0 && c->isRunning()) {
        uint16_t const program_counter = this->c->cpu.getProgramCounter();
        uint32_t const instruction = this->c->memory.loadInstruction(program_counter);
        this->c->cpu.execute(instruction);
    }

    // 2. Display the GPU frame buffer (render the current VRAM contents)
    this->c->gpu.renderFrame();

    // Calculate the elapsed time for this iteration
    auto iterationEnd = std::chrono::steady_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(iterationEnd - iterationStart).count();

    // Target frame time for 60 FPS is ~16.667 ms per frame
    constexpr double targetFrameTimeMs = 16.667;

    // If the iteration finished too quickly, delay the next iteration accordingly
    if (elapsedMs < targetFrameTimeMs) {
        std::chrono::duration<double, std::milli> sleepDuration(targetFrameTimeMs - elapsedMs);
        std::this_thread::sleep_for(sleepDuration);
    }
}
