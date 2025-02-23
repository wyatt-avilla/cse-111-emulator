#include <string>

#pragma once

#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"


class Console {
  public:
    Console();
    ~Console();
    CPU cpu;
    OS os;
    Memory memory;

    void pollInput(); // Update controller state
    uint8_t getControllerState() const; // Return controller state byte

    GPU gpu;

    void run(const std::string& slug_file_path);
    void stopExecution();
    [[nodiscard]] bool isRunning() const;

  private:
    bool is_running{true};
    uint8_t controllerState;
};
