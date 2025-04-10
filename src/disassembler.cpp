#include "disassembler.h"

#include "cpu.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

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
const uint16_t OPCODE_SHIFT = 26;
const uint16_t REG_A_SHIFT = 21;
const uint16_t REG_B_SHIFT = 16;
const uint16_t REG_C_SHIFT = 11;
const uint16_t SHIFT_VALUE_SHIFT = 6;
const uint16_t INC_FOUR_BYTES = 0x10;
const int SLUG_EXT_LENGTH = 5;
const int SHIFT_THREE_BYTES = 24;
const int SHIFT_TWO_BYTES = 16;
const int SHIFT_ONE_BYTE = 8;
const uint8_t RTYPE_OPCODE = 62;

const std::unordered_map<uint8_t, std::string> I_TYPES = {
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

const std::unordered_map<uint8_t, std::string> R_TYPES = {
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

const std::unordered_map<uint8_t, std::string> REG_NAMES = {
    {0, "ZERO"},
    {29, "STK_PTR"}
};

Disassembler::Disassembler(std::string filename)
    : filename(std::move(filename)) {}

std::string Disassembler::disassemble() {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) {
        throw std::invalid_argument("couldn't open " + filename);
    }

    std::array<uint8_t, SLUG_FILE_SIZE> slug_file{};
    file.read(reinterpret_cast<char*>(slug_file.data()), SLUG_FILE_SIZE);

    const std::string disassembled_filename = getNewFilename();

    std::ofstream disassembled(disassembled_filename);
    if (!disassembled.is_open()) {
        throw std::runtime_error(
            "couldn't open file for writing in current directory"
        );
    }

    for (int i = COPYRIGHT_MSG_BEGIN; i < COPYRIGHT_MSG_END; i++) {
        if ((slug_file[i] >= READABLE_RANGE_BEGIN &&
             slug_file[i] <= READABLE_RANGE_END) ||
            slug_file[i] == '\n')
            disassembled << (char) slug_file[i];
    }

    disassembled << std::endl << std::endl;

    parseInstructions(disassembled, slug_file);
    parseData(disassembled, slug_file);

    disassembled.close();
    return disassembled_filename;
}

std::string Disassembler::getNewFilename() {
    std::string disassembled_filename =
        filename.substr(filename.find_last_of('/') + 1);
    disassembled_filename.erase(
        disassembled_filename.length() - SLUG_EXT_LENGTH
    );
    disassembled_filename += ".S";
    return disassembled_filename;
}

void Disassembler::parseInstructions(
    std::ofstream& disassembled,
    const std::array<uint8_t, SLUG_FILE_SIZE>& slug_file
) {
    const uint16_t setup_address =
        ((uint16_t) slug_file[SETUP_ADR_LOCATION] << SHIFT_ONE_BYTE |
         slug_file[SETUP_ADR_LOCATION + 1]) -
        SLUG_FILE_SIZE;
    const uint16_t loop_address =
        ((uint16_t) slug_file[LOOP_ADR_LOCATION] << SHIFT_ONE_BYTE |
         slug_file[LOOP_ADR_LOCATION + 1]) -
        SLUG_FILE_SIZE;
    const uint16_t rom_address =
        ((uint16_t) slug_file[ROM_ADR_LOCATION] << SHIFT_ONE_BYTE |
         slug_file[ROM_ADR_LOCATION + 1]) -
        SLUG_FILE_SIZE;

    for (uint16_t i = MISC_FUNCTIONS_BEGIN; i < rom_address; i += 4) {
        if (i == setup_address)
            disassembled << std::endl << "setup:" << std::endl;
        else if (i == loop_address)
            disassembled << std::endl << "loop:" << std::endl;
        disassembled << std::hex << i + SLUG_FILE_SIZE << "\t\t" << std::dec;
        uint32_t instruction = (uint32_t) slug_file[i] << SHIFT_THREE_BYTES;
        instruction |= (uint32_t) slug_file[i + 1] << SHIFT_TWO_BYTES;
        instruction |= (uint32_t) slug_file[i + 2] << SHIFT_ONE_BYTE;
        instruction |= (uint32_t) slug_file[i + 3];

        const auto opcode =
            (uint8_t) ((instruction & OPCODE_MASK) >> OPCODE_SHIFT);
        if (opcode == RTYPE_OPCODE) {
            parseRType(disassembled, instruction);
        } else if (I_TYPES.find(opcode) != I_TYPES.end()) {
            parseIType(disassembled, instruction);
        } else {
            disassembled << "NOP" << std::endl;
        }
    }
}

void Disassembler::parseData(
    std::ofstream& disassembled,
    const std::array<uint8_t, SLUG_FILE_SIZE>& slug_file
) {
    const uint16_t rom_address =
        ((uint16_t) slug_file[ROM_ADR_LOCATION] << SHIFT_ONE_BYTE |
         slug_file[ROM_ADR_LOCATION + 1]) -
        SLUG_FILE_SIZE;
    uint32_t data_size =
        ((uint32_t) slug_file[DATA_SIZE_LOCATION] << SHIFT_THREE_BYTES |
         slug_file[DATA_SIZE_LOCATION + 1] << SHIFT_TWO_BYTES);
    data_size |=
        ((uint16_t) slug_file[DATA_SIZE_LOCATION + 2] << SHIFT_ONE_BYTE |
         slug_file[DATA_SIZE_LOCATION + 3]);
    const uint16_t data_limit = rom_address + data_size;

    disassembled << std::endl << ".data:" << std::endl << std::hex;

    for (uint16_t i = rom_address; i < data_limit; i += INC_FOUR_BYTES) {
        disassembled << i + SLUG_FILE_SIZE << "\t\t";
        for (uint16_t bit = i; bit < data_limit && bit < i + INC_FOUR_BYTES;
             bit++) {
            disassembled << static_cast<int>(slug_file[bit]) << " ";
        }
        disassembled << std::endl;
    }
}

void Disassembler::parseRType(
    std::ofstream& disassembled,
    const uint32_t& instruction
) {
    const auto reg_a = (uint8_t) ((instruction & REG_A_MASK) >> REG_A_SHIFT);
    const auto reg_b = (uint8_t) ((instruction & REG_B_MASK) >> REG_B_SHIFT);
    const auto reg_c = (uint8_t) ((instruction & REG_C_MASK) >> REG_C_SHIFT);
    const auto shift_value =
        (uint8_t) ((instruction & SHIFT_VALUE_MASK) >> SHIFT_VALUE_SHIFT);
    const auto function = (uint8_t) (instruction & FUNCTION_MASK);
    if (R_TYPES.find(function) != R_TYPES.end()) {
        disassembled << R_TYPES.at(function) << " ";
        if (REG_NAMES.find(reg_a) != REG_NAMES.end()) {
            disassembled << REG_NAMES.at(reg_a) << ",";
        } else {
            disassembled << "r" << static_cast<int>(reg_a) << ",";
        }
        if (REG_NAMES.find(reg_b) != REG_NAMES.end()) {
            disassembled << REG_NAMES.at(reg_b) << ",";
        } else {
            disassembled << "r" << static_cast<int>(reg_b) << ",";
        }
        if (REG_NAMES.find(reg_c) != REG_NAMES.end()) {
            disassembled << REG_NAMES.at(reg_c);
        } else {
            disassembled << "r" << static_cast<int>(reg_c);
        }
        if (shift_value != 0)
            disassembled << "," << static_cast<int>(shift_value);
        disassembled << std::endl;
    } else {
        disassembled << "NOP" << std::endl;
    }
}

void Disassembler::parseIType(
    std::ofstream& disassembled,
    const uint32_t& instruction
) {
    const auto opcode = (uint8_t) ((instruction & OPCODE_MASK) >> OPCODE_SHIFT);
    const auto reg_a = (uint8_t) ((instruction & REG_A_MASK) >> REG_A_SHIFT);
    const auto reg_b = (uint8_t) ((instruction & REG_B_MASK) >> REG_B_SHIFT);
    const auto immediate = (uint16_t) (instruction & IMMEDIATE_MASK);
    disassembled << I_TYPES.at(opcode) << " ";
    if (REG_NAMES.find(reg_a) != REG_NAMES.end()) {
        disassembled << REG_NAMES.at(reg_a) << ",";
    } else {
        disassembled << "r" << static_cast<int>(reg_a) << ",";
    }
    if (REG_NAMES.find(reg_b) != REG_NAMES.end()) {
        disassembled << REG_NAMES.at(reg_b) << ",";
    } else {
        disassembled << "r" << static_cast<int>(reg_b) << ",";
    }
    disassembled << static_cast<int>(immediate) << std::endl;
}