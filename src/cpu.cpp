#include "cpu.h"

#include <cstdlib>

CPU::CPU() {
    jump_table[0] = &CPU::BEQ;
    jump_table[2] = &CPU::L16;
    jump_table[16] = &CPU::L8U;
    jump_table[36] = &CPU::J;
    jump_table[48] = &CPU::S16;
    jump_table[50] = &CPU::S8;
    jump_table[54] = &CPU::ADDI;
    jump_table[59] = &CPU::BNE;
    jump_table[61] = &CPU::JAL;

    jump_table[0] = &CPU::SUB;
    jump_table[4] = &CPU::OR;
    jump_table[7] = &CPU::NOR;
    jump_table[9] = &CPU::ADD;
    jump_table[11] = &CPU::SRA;
    jump_table[19] = &CPU::XOR;
    jump_table[24] = &CPU::AND;
    jump_table[28] = &CPU::JR;
    jump_table[32] = &CPU::SLL;
    jump_table[35] = &CPU::SRL;
    jump_table[36] = &CPU::SLT;
}

void CPU::BEQ() {
    if (registers[instruction_context.reg_a] ==
        registers[instruction_context.reg_b]) {
        program_counter =
            program_counter + 4 + 4 * instruction_context.immediate;
    }
}

void CPU::L16() {
    // TODO: needs memory
}

void CPU::L8U() {
    // TODO: needs memory
}

void CPU::J() { program_counter = 4 * instruction_context.immediate; }

void CPU::S16() {
    // TODO: needs memory
}

void CPU::S8() {
    // TODO: needs memory
}

void CPU::ADDI() {
    registers[instruction_context.reg_b] =
        registers[instruction_context.reg_a] + instruction_context.immediate;
}

void CPU::BNE() {
    if (registers[instruction_context.reg_a] !=
        registers[instruction_context.reg_b]) {
        program_counter =
            program_counter + 4 + 4 * instruction_context.immediate;
    }
}

void CPU::JAL() {
    registers[31] = program_counter + 4;
    program_counter = 4 * instruction_context.immediate;
}

void CPU::SUB() {
    registers[instruction_context.reg_c] =
        registers[instruction_context.reg_a] -
        registers[instruction_context.reg_b];
}

void CPU::OR() {
    registers[instruction_context.reg_c] =
        registers[instruction_context.reg_a] |
        registers[instruction_context.reg_b];
}

void CPU::NOR() {
    registers[instruction_context.reg_c] =
        ~(registers[instruction_context.reg_a] |
          registers[instruction_context.reg_b]);
}

void CPU::ADD() {
    registers[instruction_context.reg_c] =
        registers[instruction_context.reg_a] +
        registers[instruction_context.reg_b];
}

void CPU::SRA() {
    registers[instruction_context.reg_c] =
        (signed) registers[instruction_context.reg_b] >>
        instruction_context.shift_value;
}

void CPU::XOR() {
    registers[instruction_context.reg_c] =
        registers[instruction_context.reg_a] ^
        registers[instruction_context.reg_b];
}

void CPU::AND() {
    registers[instruction_context.reg_c] =
        registers[instruction_context.reg_a] &
        registers[instruction_context.reg_b];
}

void CPU::JR() { program_counter = registers[instruction_context.reg_a]; }

void CPU::SLL() {
    registers[instruction_context.reg_c] = registers[instruction_context.reg_b]
                                           << instruction_context.shift_value;
}

void CPU::SRL() {
    registers[instruction_context.reg_c] =
        (unsigned) registers[instruction_context.reg_b] >>
        instruction_context.shift_value;
}

void CPU::SLT() {
    registers[instruction_context.reg_c] =
        (registers[instruction_context.reg_a] <
         registers[instruction_context.reg_b]);
}
