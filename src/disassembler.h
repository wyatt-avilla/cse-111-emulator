#pragma once

#include <cstdint>
#include <iostream>
#include <string>

const uint16_t SLUG_FILE_SIZE = 0x8000;

class Disassembler {
  public:
    Disassembler(std::string filename);

    std::string disassemble();

  private:
    std::string filename;

    std::string getNewFilename();
    static void parseInstructions(
        std::ofstream& disassembled,
        const std::array<uint8_t, SLUG_FILE_SIZE>& slug_file
    );
    static void parseData(
        std::ofstream& disassembled,
        const std::array<uint8_t, SLUG_FILE_SIZE>& slug_file
    );
    static void
    parseRType(std::ofstream& disassembled, const uint32_t& instruction);
    static void
    parseIType(std::ofstream& disassembled, const uint32_t& instruction);
};