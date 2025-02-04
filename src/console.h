#pragma once

#include "cpu.h"
#include "os.h"
#include "banana_memory.h"

#include <string>

class Console {
  public:
    Console(const std::string& slug_file_path);

    CPU cpu;
    OS os;
    BananaMemory memory;
    

  private:
};
