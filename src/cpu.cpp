#include "console.h"

#include <cstdlib>
#include <iostream>

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


CPU::CPU(Console* console) : console(console) {}

void CPU::execute(uint32_t instruction) {
    const uint8_t OPCODE_SHIFT = 26;
    uint16_t const OPCODE = instruction >> OPCODE_SHIFT;
    if (OPCODE == static_cast<uint16_t>(Opcode::RTYPE)) {
        const uint16_t FIRST_SIX_BITS_MASK = 0x3f;
        uint16_t const FUNCTION = instruction & FIRST_SIX_BITS_MASK;
        executeTypeR(instruction);
        if (FUNCTION != static_cast<uint16_t>(Opcode::JR)) {
            program_counter += PC_INCREMENT;
        }
    } else {
        executeTypeI(instruction);
        if (OPCODE != static_cast<uint16_t>(Opcode::BEQ) &&
            OPCODE != static_cast<uint16_t>(Opcode::J) &&
            OPCODE != static_cast<uint16_t>(Opcode::BNE) &&
            OPCODE != static_cast<uint16_t>(Opcode::JAL)) {
            program_counter += PC_INCREMENT;
        }
    }

    if (registers[ZERO_REG] != 0) {
        std::cerr << "zero register wasn't zero" << std::endl;
        registers[ZERO_REG] = 0;
    }
}

void CPU::setProgramCounterTo(uint16_t counter_value) {
    this->program_counter = counter_value;
}

uint16_t CPU::getProgramCounter() const { return this->program_counter; }

void CPU::setStackPointerTo(uint16_t pointer_value) {
    this->registers[STACK_PTR_REG] = pointer_value;
}


void CPU::executeTypeI(uint32_t instruction) {
    auto* parsed_instruction =
        reinterpret_cast<ITypeInstruction*>(&instruction);

    const auto& func_variant = I_TYPE_JUMP_TABLE[parsed_instruction->opcode];
    if (func_variant.valueless_by_exception()) {
        return;
    }

    std::visit(
        Overloaded{
            [&](RegularIType wrapper) {
                (this->*wrapper.func)(
                    parsed_instruction->reg_a,
                    parsed_instruction->reg_b,
                    parsed_instruction->immediate
                );
            },
            [&](OnlyImmediateIType wrapper) {
                (this->*wrapper.func)(parsed_instruction->immediate);
            }
        },
        func_variant
    );
}

void CPU::executeTypeR(uint32_t instruction) {
    auto* parsed_instruction =
        reinterpret_cast<RTypeInstruction*>(&instruction);

    const auto& func_variant = R_TYPE_JUMP_TABLE[parsed_instruction->function];
    if (func_variant.valueless_by_exception()) {
        return;
    }

    std::visit(
        Overloaded{
            [&](RegularRType wrapper) {
                (this->*wrapper.func)(
                    parsed_instruction->reg_a,
                    parsed_instruction->reg_b,
                    parsed_instruction->reg_c
                );
            },
            [&](JumpRegisterRType wrapper) {
                (this->*wrapper.func)(parsed_instruction->reg_a);
            },
            [&](ShiftRType wrapper) {
                (this->*wrapper.func)(
                    parsed_instruction->reg_b,
                    parsed_instruction->reg_c,
                    parsed_instruction->shift_value
                );
            }
        },
        func_variant
    );
}

void CPU::BEQ(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    if (registers[reg_a] == registers[reg_b]) {
        program_counter = program_counter + 4 + 4 * immediate;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::L16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    uint32_t const EFFECTIVE_ADDRESS = registers[reg_a] + immediate;
    registers[reg_b] = this->console->memory.l16u(EFFECTIVE_ADDRESS);
}


void CPU::L8U(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint32_t const EFFECTIVE_ADDRESS = registers[reg_a] + immediate;
        registers[reg_b] =
            static_cast<uint16_t>(this->console->memory.l8u(EFFECTIVE_ADDRESS));
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to load out of bounds." << std::endl;
    }
}


void CPU::J(uint16_t immediate) { program_counter = 4 * immediate; }

void CPU::S16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint16_t const EFFECTIVE_ADDRESS = registers[reg_a] + immediate;
        this->console->memory.w16u(EFFECTIVE_ADDRESS, registers[reg_b]);
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to store out of bounds." << std::endl;
    }
}

void CPU::S8(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    try {
        uint16_t const EFFECTIVE_ADDRESS = registers[reg_a] + immediate;
        this->console->memory.w8u(EFFECTIVE_ADDRESS, registers[reg_b]);
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to store out of bounds." << std::endl;
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

void CPU::JAL(uint16_t immediate) {
    registers[JAL_REG] = program_counter + PC_INCREMENT;
    program_counter = 4 * immediate;
}

void CPU::SUB(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] - registers[reg_b];
}

void CPU::OR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] | registers[reg_b];
}

void CPU::NOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = ~(registers[reg_a] | registers[reg_b]);
}

void CPU::ADD(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] + registers[reg_b];
}

void CPU::SRA(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value) {
    registers[reg_c] = (signed) registers[reg_b] >> shift_value;
}

void CPU::XOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] ^ registers[reg_b];
}

void CPU::AND(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] & registers[reg_b];
}

void CPU::JR(uint16_t reg_a) { program_counter = registers[reg_a]; }

void CPU::SLL(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value) {
    registers[reg_c] = registers[reg_b] << shift_value;
}

void CPU::SRL(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value) {
    registers[reg_c] = (unsigned) registers[reg_b] >> shift_value;
}

void CPU::SLT(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] =
        static_cast<uint16_t>(registers[reg_a] < registers[reg_b]);
}
