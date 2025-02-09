#include "console.h"

#include <cstdlib>
#include <iostream>

#define PC_INCREMENT 4
#define STK_PTR_REG 29
#define ZERO_REG 0
#define JAL_REG 31

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


CPU::CPU(Console* console) : console(console) {
    i_type_jump_table[static_cast<uint16_t>(Opcode::BEQ)] = &CPU::BEQ;
    i_type_jump_table[static_cast<uint16_t>(Opcode::L16)] = &CPU::L16;
    i_type_jump_table[static_cast<uint16_t>(Opcode::L8U)] = &CPU::L8U;
    i_type_jump_table[static_cast<uint16_t>(Opcode::J)] = &CPU::J;
    i_type_jump_table[static_cast<uint16_t>(Opcode::S16)] = &CPU::S16;
    i_type_jump_table[static_cast<uint16_t>(Opcode::S8)] = &CPU::S8;
    i_type_jump_table[static_cast<uint16_t>(Opcode::ADDI)] = &CPU::ADDI;
    i_type_jump_table[static_cast<uint16_t>(Opcode::BNE)] = &CPU::BNE;
    i_type_jump_table[static_cast<uint16_t>(Opcode::JAL)] = &CPU::JAL;

    r_type_jump_table[static_cast<uint16_t>(Opcode::SUB)] = &CPU::SUB;
    r_type_jump_table[static_cast<uint16_t>(Opcode::OR)] = &CPU::OR;
    r_type_jump_table[static_cast<uint16_t>(Opcode::NOR)] = &CPU::NOR;
    r_type_jump_table[static_cast<uint16_t>(Opcode::ADD)] = &CPU::ADD;
    r_type_jump_table[static_cast<uint16_t>(Opcode::SRA)] = &CPU::SRA;
    r_type_jump_table[static_cast<uint16_t>(Opcode::XOR)] = &CPU::XOR;
    r_type_jump_table[static_cast<uint16_t>(Opcode::AND)] = &CPU::AND;
    r_type_jump_table[static_cast<uint16_t>(Opcode::JR)] = &CPU::JR;
    r_type_jump_table[static_cast<uint16_t>(Opcode::SLL)] = &CPU::SLL;
    r_type_jump_table[static_cast<uint16_t>(Opcode::SRL)] = &CPU::SRL;
    r_type_jump_table[static_cast<uint16_t>(Opcode::SLT)] = &CPU::SLT;
}


void CPU::execute(uint32_t instruction) {
    // std::cerr << std::hex << instruction << std::endl;
    uint16_t opcode = instruction >> 26;
    if (opcode == static_cast<uint16_t>(Opcode::RTYPE)) {
        const uint16_t first_six_bits_mask = 0x3f;
        uint16_t function = instruction & first_six_bits_mask;
        executeTypeR(instruction);
        if (function != static_cast<uint16_t>(Opcode::JR)) {
            program_counter += PC_INCREMENT;
        }
    } else {
        executeTypeI(instruction);
        if (opcode != static_cast<uint16_t>(Opcode::BEQ) &&
            opcode != static_cast<uint16_t>(Opcode::J) &&
            opcode != static_cast<uint16_t>(Opcode::BNE) &&
            opcode != static_cast<uint16_t>(Opcode::JAL)) {
            program_counter += PC_INCREMENT;
        }
    }

    if (registers[ZERO_REG] != 0) {
        std::cerr << "zero register wasn't zero" << std::endl;
        registers[ZERO_REG] = 0;
    }
}

void CPU::set_program_counter_to(uint16_t counter_value) {
    this->program_counter = counter_value;
}

uint16_t CPU::get_program_counter() { return this->program_counter; }

void CPU::set_stack_pointer_to(uint16_t pointer_value) {
    this->registers[STK_PTR_REG] = pointer_value;
}


void CPU::executeTypeI(uint32_t instruction) {
    ITypeInstruction* parsed_instruction =
        reinterpret_cast<ITypeInstruction*>(&instruction);

    if (i_type_jump_table[parsed_instruction->opcode] != nullptr) {
        (this->*i_type_jump_table[parsed_instruction->opcode])(
            parsed_instruction->reg_a,
            parsed_instruction->reg_b,
            parsed_instruction->immediate
        );
    }
}

void CPU::executeTypeR(uint32_t instruction) {
    RTypeInstruction* parsed_instruction =
        reinterpret_cast<RTypeInstruction*>(&instruction);

    if (r_type_jump_table[parsed_instruction->function] != nullptr) {
        (this->*r_type_jump_table[parsed_instruction->function])(
            parsed_instruction->reg_a,
            parsed_instruction->reg_b,
            parsed_instruction->reg_c,
            parsed_instruction->shift_value
        );
    }
}

void CPU::BEQ(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    if (registers[reg_a] == registers[reg_b]) {
        program_counter = program_counter + 4 + 4 * immediate;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::L16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint32_t effective_address = registers[reg_a] + immediate;
        registers[reg_b] = this->console->memory.l16u(effective_address);
    } catch (const std::invalid_argument&) {
    }
}


void CPU::L8U(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint32_t effective_address = registers[reg_a] + immediate;
        registers[reg_b] =
            static_cast<uint16_t>(this->console->memory.l8u(effective_address));
    } catch (const std::invalid_argument&) {
    }
}


void CPU::J(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    program_counter = 4 * immediate;
}

void CPU::S16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint16_t effective_address = registers[reg_a] + immediate;
        this->console->memory.w16u(effective_address, registers[reg_b]);
    } catch (const std::invalid_argument&) {
    }
}

void CPU::S8(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint16_t effective_address = registers[reg_a] + immediate;
        this->console->memory.w8u(effective_address, registers[reg_b]);
    } catch (const std::invalid_argument&) {
    }
}

void CPU::ADDI(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    registers[reg_b] = registers[reg_a] + immediate;
}

void CPU::BNE(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    if (registers[reg_a] != registers[reg_b]) {
        program_counter = program_counter + 4 + 4 * immediate;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::JAL(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    registers[JAL_REG] = program_counter + PC_INCREMENT;
    program_counter = 4 * immediate;
}

void CPU::JAL(uint16_t immediate) { JAL(0, 0, immediate); }

void CPU::SUB(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = registers[reg_a] - registers[reg_b];
}

void CPU::OR(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = registers[reg_a] | registers[reg_b];
}

void CPU::NOR(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = ~(registers[reg_a] | registers[reg_b]);
}

void CPU::ADD(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = registers[reg_a] + registers[reg_b];
}

void CPU::SRA(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = (signed) registers[reg_b] >> shift_value;
}

void CPU::XOR(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = registers[reg_a] ^ registers[reg_b];
}

void CPU::AND(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = registers[reg_a] & registers[reg_b];
}

void CPU::JR(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    program_counter = registers[reg_a];
}

void CPU::SLL(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = registers[reg_b] << shift_value;
}

void CPU::SRL(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = (unsigned) registers[reg_b] >> shift_value;
}

void CPU::SLT(
    uint16_t reg_a,
    uint16_t reg_b,
    uint16_t reg_c,
    uint16_t shift_value
) {
    registers[reg_c] = (registers[reg_a] < registers[reg_b]);
}
