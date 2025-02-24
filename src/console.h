
#include <string>

#pragma once

#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"


class Console {
  public:
    Console();
    CPU cpu;
    OS os;
    Memory memory;
    GPU gpu;

    void run(const std::string& slug_file_path);
    void stopExecution();
    [[nodiscard]] bool isRunning() const;

  private:
    bool is_running{true};
};