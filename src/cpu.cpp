#include "cpu.h"

#include <cstdlib>

struct ITypeInstruction {
    uint16_t immediate : 16;
    uint16_t reg_b : 5;
    uint16_t reg_a : 5;
    uint16_t opcode : 6;
};

struct RTypeInstruction {
    uint16_t function : 6;
    uint16_t shift_value : 5;
    uint16_t reg_c : 5;
    uint16_t reg_b : 5;
    uint16_t reg_a : 5;
    uint16_t opcode : 6;
};


CPU::CPU() {
    jump_table[static_cast<uint16_t>(Opcode::BEQ)] = &CPU::BEQ;
    jump_table[static_cast<uint16_t>(Opcode::L16)] = &CPU::L16;
    jump_table[static_cast<uint16_t>(Opcode::L8U)] = &CPU::L8U;
    jump_table[static_cast<uint16_t>(Opcode::J)] = &CPU::J;
    jump_table[static_cast<uint16_t>(Opcode::S16)] = &CPU::S16;
    jump_table[static_cast<uint16_t>(Opcode::S8)] = &CPU::S8;
    jump_table[static_cast<uint16_t>(Opcode::ADDI)] = &CPU::ADDI;
    jump_table[static_cast<uint16_t>(Opcode::BNE)] = &CPU::BNE;
    jump_table[static_cast<uint16_t>(Opcode::JAL)] = &CPU::JAL;

    jump_table[static_cast<uint16_t>(Opcode::SUB)] = &CPU::SUB;
    jump_table[static_cast<uint16_t>(Opcode::OR)] = &CPU::OR;
    jump_table[static_cast<uint16_t>(Opcode::NOR)] = &CPU::NOR;
    jump_table[static_cast<uint16_t>(Opcode::ADD)] = &CPU::ADD;
    jump_table[static_cast<uint16_t>(Opcode::SRA)] = &CPU::SRA;
    jump_table[static_cast<uint16_t>(Opcode::XOR)] = &CPU::XOR;
    jump_table[static_cast<uint16_t>(Opcode::AND)] = &CPU::AND;
    jump_table[static_cast<uint16_t>(Opcode::JR)] = &CPU::JR;
    jump_table[static_cast<uint16_t>(Opcode::SLL)] = &CPU::SLL;
    jump_table[static_cast<uint16_t>(Opcode::SRL)] = &CPU::SRL;
    jump_table[static_cast<uint16_t>(Opcode::SLT)] = &CPU::SLT;
}


void CPU::execute(uint32_t instruction) {
    uint16_t opcode = instruction >> 26;
    if (opcode == static_cast<uint16_t>(Opcode::RTYPE)) {
        executeTypeR(instruction);
        if (opcode != static_cast<uint16_t>(Opcode::BEQ) &&
            opcode != static_cast<uint16_t>(Opcode::BNE) &&
            opcode != static_cast<uint16_t>(Opcode::JAL)) {
            program_counter += 4;
        }
    } else {
        executeTypeI(instruction);
        if (opcode != static_cast<uint16_t>(Opcode::JR)) {
            program_counter += 4;
        }
    }
}

void CPU::executeTypeI(uint32_t instruction) {
    ITypeInstruction* parsed_instruction =
        reinterpret_cast<ITypeInstruction*>(&instruction);
    instruction_context.immediate = parsed_instruction->immediate;
    instruction_context.reg_a = parsed_instruction->reg_a;
    instruction_context.reg_b = parsed_instruction->reg_b;

    if (jump_table[parsed_instruction->opcode] != nullptr) {
        (this->*jump_table[parsed_instruction->opcode])();
    }
}

void CPU::executeTypeR(uint32_t instruction) {
    RTypeInstruction* parsed_instruction =
        reinterpret_cast<RTypeInstruction*>(&instruction);
    instruction_context.reg_a = parsed_instruction->reg_a;
    instruction_context.reg_b = parsed_instruction->reg_b;
    instruction_context.reg_c = parsed_instruction->reg_c;
    instruction_context.shift_value = parsed_instruction->shift_value;

    if (jump_table[parsed_instruction->function] != nullptr) {
        (this->*jump_table[parsed_instruction->opcode])();
    }
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
