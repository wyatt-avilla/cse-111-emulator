#include <array>
#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "disassembler.h"
#include "cpu.h"

const uint16_t SLUG_FILE_SIZE = 0x8000;
const uint16_t COPYRIGHT_MSG_BEGIN = 0x10;
const uint16_t COPYRIGHT_MSG_END = 0x1D2;
const uint16_t READABLE_RANGE_BEGIN = 0x20;
const uint16_t READABLE_RANGE_END = 0x7E;
const uint16_t SETUP_ADR_LOCATION = 0x1E2;
const uint16_t LOOP_ADR_LOCATION = 0x1E6;
const uint16_t ROM_ADR_LOCATION = 0x1EA;
const uint32_t OPCODE_MASK = 0xFC000000;
const uint32_t REG_A_MASK = 0x3E00000;
const uint32_t REG_B_MASK = 0x1F0000;
const uint16_t REG_C_MASK = 0xF800;
const uint16_t SHIFT_VALUE_MASK = 0x7C0;
const uint8_t FUNCTION_MASK = 0x3F;
const uint16_t IMMEDIATE_MASK = 0xFFFF;
const uint16_t MISC_FUNCTIONS_BEGIN = 0x200;
const uint16_t DATA_SIZE_LOCATION = 0x1F0;

const std::unordered_map<uint8_t, std::string> i_types = {
  {0, "BEQ"},
  {2, "L16"},
  {16, "L8U"},
  {36, "J"},
  {48, "S16"},
  {50, "S8"},
  {54, "ADDI"},
  {59, "BNE"},
  {61, "JAL"}
};

const std::unordered_map<uint8_t, std::string> r_types = {
  {0, "SUB"},
  {4, "OR"},
  {7, "NOR"},
  {9, "ADD"},
  {11, "SRA"},
  {19, "XOR"},
  {24, "AND"},
  {28, "JR"},
  {32, "SLL"},
  {35, "SRL"},
  {36, "SLT"}
};

const std::unordered_map<uint8_t, std::string> reg_names = {
  {0, "ZERO"},
  {29, "STK_PTR"}
};

Disassembler::Disassembler(const std::string& filename) {
  this->filename = filename;
}

std::string Disassembler::disassemble() {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open()) {
    throw std::invalid_argument("couldn't open " + filename);
  }

  std::string disassembled_filename = filename.substr(filename.find_last_of('/')+1);
  disassembled_filename.erase(disassembled_filename.length() - 5);
  disassembled_filename += ".S";

  std::array<uint8_t, SLUG_FILE_SIZE> slug_file {};
  file.read(reinterpret_cast<char*>(slug_file.data()), SLUG_FILE_SIZE);

  std::ofstream disassembled (disassembled_filename);
  if (!disassembled.is_open()) {
    throw std::runtime_error("couldn't open file for writing in current directory");
  }

  for (int i = COPYRIGHT_MSG_BEGIN; i < COPYRIGHT_MSG_END; i++) {
    if ((slug_file[i] >= READABLE_RANGE_BEGIN && slug_file[i] <= READABLE_RANGE_END) || slug_file[i] == '\n')
      disassembled << (char) slug_file[i];
  }

  disassembled << std::endl << std::endl;

  uint16_t setup_address = ((uint16_t) slug_file[SETUP_ADR_LOCATION] << 8 | slug_file[SETUP_ADR_LOCATION+1]) - SLUG_FILE_SIZE;
  uint16_t loop_address = ((uint16_t) slug_file[LOOP_ADR_LOCATION] << 8 | slug_file[LOOP_ADR_LOCATION+1]) - SLUG_FILE_SIZE;
  uint16_t rom_address = ((uint16_t) slug_file[ROM_ADR_LOCATION] << 8 | slug_file[ROM_ADR_LOCATION+1]) - SLUG_FILE_SIZE;
  uint32_t data_size = ((uint32_t) slug_file[DATA_SIZE_LOCATION] << 24 | slug_file[DATA_SIZE_LOCATION+1] << 16);
  data_size |= ((uint16_t) slug_file[DATA_SIZE_LOCATION+2] << 8 | slug_file[DATA_SIZE_LOCATION+3]);

  for (uint16_t i = MISC_FUNCTIONS_BEGIN; i < rom_address; i += 4) {
    if (i == setup_address)
      disassembled << std::endl << "setup:" << std::endl;
    else if (i == loop_address)
      disassembled << std::endl << "loop:" << std::endl;
    disassembled << std::hex << i + SLUG_FILE_SIZE << "\t\t" << std::dec;
    uint32_t instruction = (uint32_t) slug_file[i] << 24;
    instruction |= (uint32_t) slug_file[i+1] << 16;
    instruction |= (uint32_t) slug_file[i+2] << 8;
    instruction |= (uint32_t) slug_file[i+3];

    uint8_t opcode = (uint8_t) ((instruction & OPCODE_MASK) >> 26);
    uint8_t reg_a = (uint8_t) ((instruction & REG_A_MASK) >> 21);
    uint8_t reg_b = (uint8_t) ((instruction & REG_B_MASK) >> 16);
    if (opcode == 62) {
      uint8_t reg_c = (uint8_t) ((instruction & REG_C_MASK) >> 11);
      int8_t shift_value = (uint8_t) ((instruction & SHIFT_VALUE_MASK) >> 6);
      uint8_t function = (uint8_t) (instruction & FUNCTION_MASK);
      if (r_types.find(function) != r_types.end()) {
        disassembled << r_types.at(function) << " ";
        if (reg_names.find(reg_a) != reg_names.end()) {
          disassembled << reg_names.at(reg_a) << ",";
        } else {
          disassembled << "r" << static_cast<int>(reg_a) << ",";
        }
        if (reg_names.find(reg_b) != reg_names.end()) {
          disassembled << reg_names.at(reg_b) << ",";
        } else {
          disassembled << "r" << static_cast<int>(reg_b) << ",";
        }
        if (reg_names.find(reg_c) != reg_names.end()) {
          disassembled << reg_names.at(reg_c);
        } else {
          disassembled << "r" << static_cast<int>(reg_c);
        }
        if (shift_value)
          disassembled  << "," << static_cast<int>(shift_value);
        disassembled << std::endl;
      } else {
        disassembled << "NOP" << std::endl;
      }
    } else if (i_types.find(opcode) != i_types.end()) {
      int16_t immediate = (uint16_t) (instruction & IMMEDIATE_MASK);
      disassembled << i_types.at(opcode) << " ";
      if (reg_names.find(reg_a) != reg_names.end()) {
        disassembled << reg_names.at(reg_a) << ",";
      } else {
        disassembled << "r" << static_cast<int>(reg_a) << ",";
      }
      if (reg_names.find(reg_b) != reg_names.end()) {
        disassembled << reg_names.at(reg_b) << ",";
      } else {
        disassembled << "r" << static_cast<int>(reg_b) << ",";
      } 
      disassembled << static_cast<int>(immediate) << std::endl;
    } else {
      disassembled << "NOP" << std::endl;
    }
  }

  disassembled << std::endl << ".data:" << std::endl << std::hex;

  uint16_t data_limit = rom_address + data_size;

  for (uint16_t i = rom_address; i < data_limit; i += 0x10) {
    disassembled << i + SLUG_FILE_SIZE << "\t\t";
    for (uint16_t bit = i; bit < data_limit && bit < i+0x10; bit++) {
      disassembled << static_cast<int>(slug_file[bit]) << " ";
    }
    disassembled << std::endl;
  }

  disassembled.close();
  return disassembled_filename;
}