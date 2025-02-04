#pragma once

#include "cpu.h"

#include <string>

class Console {
  public:
    Console(const std::string& slug_file_path);

    CPU cpu;

  private:
};
