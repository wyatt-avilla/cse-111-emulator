#pragma once

#include "banana_memory.h"
#include "cpu.h"
#include "os.h"

#include <string>

class Console {
  public:
    Console();

    void run(const std::string& slug_file_path);

    CPU cpu;
    OS os;
    BananaMemory memory;

  private:
};
