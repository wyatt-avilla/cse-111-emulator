#include "cpu.h"

#include "console.h"

#include <cstdlib>
#include <iostream>
#include <type_traits>

struct ITypeInstructionBits {
    uint16_t immediate : 16;
    uint16_t reg_b : 5;
    uint16_t reg_a : 5;
    uint16_t opcode : 6;
};

struct RTypeInstructionBits {
    uint16_t function : 6;
    uint16_t shift_value : 5;
    uint16_t reg_c : 5;
    uint16_t reg_b : 5;
    uint16_t reg_a : 5;
    uint16_t opcode : 6;
};


CPU::CPU(Console* console) : console(console) {}

void CPU::execute(const uint32_t instruction) {
    const uint8_t opcode_shift = 26;
    const uint16_t opcode = instruction >> opcode_shift;
    const uint16_t first_six_bits_mask = 0x3f;

    if (opcode == static_cast<std::underlying_type_t<Opcode>>(Opcode::RTYPE)) {
        uint16_t const function = instruction & first_six_bits_mask;
        executeTypeR(instruction);
        if (function !=
            static_cast<std::underlying_type_t<Opcode>>(Opcode::JR)) {
            program_counter += PC_INCREMENT;
        }
    } else {
        executeTypeI(instruction);
        if (opcode !=
                static_cast<std::underlying_type_t<Opcode>>(Opcode::BEQ) &&
            opcode != static_cast<std::underlying_type_t<Opcode>>(Opcode::J) &&
            opcode !=
                static_cast<std::underlying_type_t<Opcode>>(Opcode::BNE) &&
            opcode !=
                static_cast<std::underlying_type_t<Opcode>>(Opcode::JAL)) {
            program_counter += PC_INCREMENT;
        }
    }

    if (registers[ZERO_REG] != 0) {
        std::cerr << "zero register wasn't zero" << std::endl;
        registers[ZERO_REG] = 0;
    }
}

void CPU::setProgramCounterTo(const uint16_t counter_value) {
    this->program_counter = counter_value;
}

uint16_t CPU::getProgramCounter() const { return this->program_counter; }

void CPU::setStackPointerTo(const uint16_t pointer_value) {
    this->registers[STACK_PTR_REG] = static_cast<int16_t>(pointer_value);
}


void CPU::executeTypeI(const uint32_t instruction) {
    const auto* parsed_instruction =
        reinterpret_cast<const ITypeInstructionBits*>(&instruction);

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

void CPU::executeTypeR(const uint32_t instruction) {
    const auto* parsed_instruction =
        reinterpret_cast<const RTypeInstructionBits*>(&instruction);

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
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    if (registers[reg_a] == registers[reg_b]) {
        program_counter = program_counter + 4 + 4 * immediate;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::L16(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    uint32_t const effective_address = registers[reg_a] + immediate;
    registers[reg_b] =
        static_cast<int16_t>(this->console->memory.l16u(effective_address));
}


void CPU::L8U(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    try {
        uint32_t const effective_address = registers[reg_a] + immediate;
        registers[reg_b] =
            static_cast<int16_t>(this->console->memory.l8u(effective_address));
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to load out of bounds." << std::endl;
    }
}


void CPU::J(const uint16_t immediate) { program_counter = 4 * immediate; }

void CPU::S16(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    try {
        uint16_t const effective_address = registers[reg_a] + immediate;
        this->console->memory.w16u(effective_address, registers[reg_b]);
    } catch (const std::invalid_argument&) {
        std::cerr << "Tried to store out of bounds." << std::endl;
    }
}

void CPU::S8(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    uint16_t const effective_address = registers[reg_a] + immediate;
    try {
        this->console->memory.w8u(effective_address, registers[reg_b]);
    } catch (const std::invalid_argument& e) {
        std::cerr << "CPU::S8 error: " << e.what() << std::endl;
    }
}

void CPU::ADDI(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    registers[reg_b] = static_cast<int16_t>(registers[reg_a] + immediate);
}

void CPU::BNE(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    if (registers[reg_a] != registers[reg_b]) {
        program_counter = program_counter + 4 + 4 * immediate;
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::JAL(const uint16_t immediate) {
    registers[JAL_REG] = static_cast<int16_t>(program_counter + PC_INCREMENT);
    program_counter = 4 * immediate;
}

void CPU::SUB(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<int16_t>(registers[reg_a] - registers[reg_b]);
}

void CPU::OR(const uint16_t reg_a, const uint16_t reg_b, const uint16_t reg_c) {
    registers[reg_c] =
        static_cast<int16_t>(registers[reg_a] | registers[reg_b]);
}

void CPU::NOR(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<int16_t>(~(registers[reg_a] | registers[reg_b]));
}

void CPU::ADD(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<int16_t>(registers[reg_a] + registers[reg_b]);
}

void CPU::SRA(
    const uint16_t reg_b,
    const uint16_t reg_c,
    const uint16_t shift_value
) {
    registers[reg_c] =
        static_cast<int16_t>((signed) registers[reg_b] >> shift_value);
}

void CPU::XOR(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<int16_t>(registers[reg_a] ^ registers[reg_b]);
}

void CPU::AND(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<int16_t>(registers[reg_a] & registers[reg_b]);
}

void CPU::JR(const uint16_t reg_a) { program_counter = registers[reg_a]; }

void CPU::SLL(
    const uint16_t reg_b,
    const uint16_t reg_c,
    const uint16_t shift_value
) {
    registers[reg_c] = static_cast<int16_t>(registers[reg_b] << shift_value);
}

void CPU::SRL(
    const uint16_t reg_b,
    const uint16_t reg_c,
    const uint16_t shift_value
) {
    registers[reg_c] =
        static_cast<int16_t>((unsigned) registers[reg_b] >> shift_value);
}

void CPU::SLT(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<int16_t>(registers[reg_a] < registers[reg_b]);
}