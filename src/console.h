#include <string>

#pragma once

#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"


class Console {
  public:
    Console();
    virtual ~Console(); // 🔹 Added virtual destructor for proper cleanup
    CPU cpu;
    OS os;
    Memory memory;
    GPU gpu;
    void pollInput(); // Update controller state
    uint8_t getControllerState() const;  void run(const std::string& slug_file_path);
    void stopExecution();
    [[nodiscard]] bool isRunning() const;

  private:
    bool is_running{true};
    uint8_t controllerState;


};