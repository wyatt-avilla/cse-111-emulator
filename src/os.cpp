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
    this->c->memory.clearRAM();

    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::invalid_argument("couldn't open " + filename);
    }
    this->c->memory.loadFile(file);
    file.close();

    this->c->memory.copyDataSectionToRam();

    this->c->cpu.setStackPointerTo(
        static_cast<uint16_t>(Memory::Address::STACK_END)
    );

    // NEW: Set the GPU's external VRAM pointer.
    // This assumes Memory provides a getPointerToMemArray() method (see below)
    this->c->gpu.setExternalVRAM(
        this->c->memory.getPointerToMemArray() + 0x3000
    );

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
    auto iterationStart = std::chrono::steady_clock::now();

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

    auto iterationEnd = std::chrono::steady_clock::now();
    double elapsedMs =
        std::chrono::duration<double, std::milli>(iterationEnd - iterationStart)
            .count();
    constexpr double targetFrameTimeMs = 16.667;
    if (elapsedMs < targetFrameTimeMs) {
        std::chrono::duration<double, std::milli> sleepDuration(
            targetFrameTimeMs - elapsedMs
        );
        std::this_thread::sleep_for(sleepDuration);
    }
}