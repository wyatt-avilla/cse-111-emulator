#include <string>

#pragma once

#include "cpu.h"
#include "memory.h"
#include "os.h"


class Console {
  public:
    Console();

    void run(const std::string& slug_file_path);
    void stopExecution();
    CPU cpu;
    OS os;
    Memory memory;

  private:
    bool is_running;
};
