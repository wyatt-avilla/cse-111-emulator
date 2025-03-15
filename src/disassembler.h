#pragma once

#include <string>

class Disassembler {
  public:
    Disassembler(const std::string& filename);
    
    std::string disassemble();

  private:
    std::string filename;
};