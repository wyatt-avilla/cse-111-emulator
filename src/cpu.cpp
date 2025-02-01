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
