#include <string>

#pragma once

#include "cpu.h"
#include "memory.h"
#include "os.h"


class Console {
  public:
    Console();

    void run(const std::string& slug_file_path);

    CPU cpu;
    OS os;
    Memory memory;

  private:
};
