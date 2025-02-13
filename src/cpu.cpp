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

void CPU::execute(const uint32_t INSTRUCTION) {
    const uint8_t OPCODE_SHIFT = 26;
    uint16_t const OPCODE = INSTRUCTION >> OPCODE_SHIFT;
    if (OPCODE == static_cast<uint16_t>(Opcode::RTYPE)) {
        const uint16_t FIRST_SIX_BITS_MASK = 0x3f;
        uint16_t const FUNCTION = INSTRUCTION & FIRST_SIX_BITS_MASK;
        executeTypeR(INSTRUCTION);
        if (FUNCTION != static_cast<uint16_t>(Opcode::JR)) {
            program_counter += PC_INCREMENT;
        }
    } else {
        executeTypeI(INSTRUCTION);
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

void CPU::setProgramCounterTo(const uint16_t COUNTER_VALUE) {
    this->program_counter = COUNTER_VALUE;
}

uint16_t CPU::getProgramCounter() const { return this->program_counter; }

void CPU::setStackPointerTo(const uint16_t POINTER_VALUE) {
    this->registers[STACK_PTR_REG] = POINTER_VALUE;
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

void CPU::BEQ(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    if (registers[REG_A] == registers[REG_B]) {
        program_counter = program_counter + 4 + 4 * IMMEDIATE;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::L16(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    uint32_t const EFFECTIVE_ADDRESS = registers[REG_A] + IMMEDIATE;
    registers[REG_B] = this->console->memory.l16u(EFFECTIVE_ADDRESS);
}


void CPU::L8U(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    try {
        uint32_t const EFFECTIVE_ADDRESS = registers[REG_A] + IMMEDIATE;
        registers[REG_B] =
            static_cast<uint16_t>(this->console->memory.l8u(EFFECTIVE_ADDRESS));
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to load out of bounds." << std::endl;
    }
}


void CPU::J(const uint16_t IMMEDIATE) { program_counter = 4 * IMMEDIATE; }

void CPU::S16(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    try {
        uint16_t const EFFECTIVE_ADDRESS = registers[REG_A] + IMMEDIATE;
        this->console->memory.w16u(EFFECTIVE_ADDRESS, registers[REG_B]);
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to store out of bounds." << std::endl;
    }
}

void CPU::S8(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    try {
        uint16_t const EFFECTIVE_ADDRESS = registers[REG_A] + IMMEDIATE;
        this->console->memory.w8u(EFFECTIVE_ADDRESS, registers[REG_B]);
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to store out of bounds." << std::endl;
    }
}

void CPU::ADDI(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    registers[REG_B] = registers[REG_A] + IMMEDIATE;
}

void CPU::BNE(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t IMMEDIATE
) {
    if (registers[REG_A] != registers[REG_B]) {
        program_counter = program_counter + 4 + 4 * IMMEDIATE;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::JAL(const uint16_t IMMEDIATE) {
    registers[JAL_REG] = program_counter + PC_INCREMENT;
    program_counter = 4 * IMMEDIATE;
}

void CPU::SUB(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t REG_C
) {
    registers[REG_C] = registers[REG_A] - registers[REG_B];
}

void CPU::OR(const uint16_t REG_A, const uint16_t REG_B, const uint16_t REG_C) {
    registers[REG_C] = registers[REG_A] | registers[REG_B];
}

void CPU::NOR(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t REG_C
) {
    registers[REG_C] = ~(registers[REG_A] | registers[REG_B]);
}

void CPU::ADD(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t REG_C
) {
    registers[REG_C] = registers[REG_A] + registers[REG_B];
}

void CPU::SRA(
    const uint16_t REG_B,
    const uint16_t REG_C,
    const uint16_t SHIFT_VALUE
) {
    registers[REG_C] = (signed) registers[REG_B] >> SHIFT_VALUE;
}

void CPU::XOR(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t REG_C
) {
    registers[REG_C] = registers[REG_A] ^ registers[REG_B];
}

void CPU::AND(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t REG_C
) {
    registers[REG_C] = registers[REG_A] & registers[REG_B];
}

void CPU::JR(const uint16_t REG_A) { program_counter = registers[REG_A]; }

void CPU::SLL(
    const uint16_t REG_B,
    const uint16_t REG_C,
    const uint16_t SHIFT_VALUE
) {
    registers[REG_C] = registers[REG_B] << SHIFT_VALUE;
}

void CPU::SRL(
    const uint16_t REG_B,
    const uint16_t REG_C,
    const uint16_t SHIFT_VALUE
) {
    registers[REG_C] = (unsigned) registers[REG_B] >> SHIFT_VALUE;
}

void CPU::SLT(
    const uint16_t REG_A,
    const uint16_t REG_B,
    const uint16_t REG_C
) {
    registers[REG_C] =
        static_cast<uint16_t>(registers[REG_A] < registers[REG_B]);
}
