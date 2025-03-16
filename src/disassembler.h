#pragma once

#include <iostream>
#include <string>

const uint16_t SLUG_FILE_SIZE = 0x8000;

class Disassembler {
  public:
    Disassembler(const std::string& filename);

    std::string disassemble();

  private:
    std::string filename;

    std::string getNewFilename();
    void parseInstructions(std::ofstream& disassembled, const std::array<uint8_t, SLUG_FILE_SIZE>& slug_file);
    void parseData(std::ofstream& disassembled, const std::array<uint8_t, SLUG_FILE_SIZE>& slug_file);
};