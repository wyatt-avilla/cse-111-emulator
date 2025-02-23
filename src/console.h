#pragma once

#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"

#include <SDL2/SDL.h>
#include <string>

class Console {
  public:
    Console();
    ~Console();

    void pollInput();
    uint8_t getControllerState() const;
    void stopExecution();
    bool isRunning() const;
    void run(const std::string& slug_file_path);

    Memory memory;
    CPU cpu;
    OS os;
    GPU gpu;

  private:
    bool is_running = true;
    uint8_t controllerState;
};
