#pragma once

#include "memory.h"
#include "cpu.h"
#include "os.h"

#include <string>

class Console {
  public:
    Console();

    void run(const std::string& slug_file_path);

    CPU cpu;
    OS os;
    Memory memory;

  private:
};
