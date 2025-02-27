#include <string>

#pragma once

#include "controller.h"
#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"

class Console {
  public:
    Console(bool graphics);
    CPU cpu;
    OS os;
    Memory memory;
    GPU gpu;
    Controller controller;

    void run(const std::string& slug_file_path);
    void stopExecution();
    [[nodiscard]] bool isRunning() const;
    [[nodiscard]] bool graphicalSession() const;

  private:
    bool is_running{true};
    bool graphics{true};
};
