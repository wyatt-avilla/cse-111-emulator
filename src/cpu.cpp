#include "cpu.h"
#include <iostream>

CPU::CPU(Console* console) : console(console) {}

void CPU::execute(const uint32_t instruction) {
    std::cerr << "🚀 Executing Instruction: " << std::hex << instruction 
              << " at PC: " << getProgramCounter() << std::endl;

    uint16_t old_pc = getProgramCounter();

    const uint8_t opcode_shift = 26;
    const uint16_t opcode = instruction >> opcode_shift;
    const uint16_t first_six_bits_mask = 0x3f;

    if (opcode == static_cast<uint16_t>(Opcode::RTYPE)) {
        uint16_t function = instruction & first_six_bits_mask;
        executeTypeR(instruction);
    } else {
        executeTypeI(instruction);
    }

    if (old_pc == getProgramCounter()) {
        std::cerr << "⚠️ [WARNING] PC is stuck at: " << std::hex << old_pc << std::endl;
    }
}

// ✅ Fixed Branching Logic
void CPU::BEQ(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    if (registers[reg_a] == registers[reg_b]) {
        program_counter += (4 + (4 * immediate));
    } else {
        program_counter += 4;
    }
}

void CPU::BNE(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    if (registers[reg_a] != registers[reg_b]) {
        program_counter += (4 + (4 * immediate));
    } else {
        program_counter += 4;
    }
}