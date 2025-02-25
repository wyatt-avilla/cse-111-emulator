/*


#include "console.h"

#include <cstdlib>
#include <iostream>

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

    if (opcode == static_cast<uint16_t>(Opcode::RTYPE)) {
        uint16_t const function = instruction & first_six_bits_mask;
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

void CPU::setProgramCounterTo(const uint16_t counter_value) {
    this->program_counter = counter_value;
}

uint16_t CPU::getProgramCounter() const { return this->program_counter; }

void CPU::setStackPointerTo(const uint16_t pointer_value) {
    this->registers[STACK_PTR_REG] = pointer_value;
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
    registers[reg_b] = this->console->memory.l16u(effective_address);
}


void CPU::L8U(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t immediate
) {
    try {
        uint32_t const effective_address = registers[reg_a] + immediate;
        registers[reg_b] =
            static_cast<uint16_t>(this->console->memory.l8u(effective_address));
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
    registers[reg_b] = registers[reg_a] + immediate;
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
    registers[JAL_REG] = program_counter + PC_INCREMENT;
    program_counter = 4 * immediate;
}

void CPU::SUB(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] = registers[reg_a] - registers[reg_b];
}

void CPU::OR(const uint16_t reg_a, const uint16_t reg_b, const uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] | registers[reg_b];
}

void CPU::NOR(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] = ~(registers[reg_a] | registers[reg_b]);
}

void CPU::ADD(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] = registers[reg_a] + registers[reg_b];
}

void CPU::SRA(
    const uint16_t reg_b,
    const uint16_t reg_c,
    const uint16_t shift_value
) {
    registers[reg_c] = (signed) registers[reg_b] >> shift_value;
}

void CPU::XOR(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] = registers[reg_a] ^ registers[reg_b];
}

void CPU::AND(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] = registers[reg_a] & registers[reg_b];
}

void CPU::JR(const uint16_t reg_a) { program_counter = registers[reg_a]; }

void CPU::SLL(
    const uint16_t reg_b,
    const uint16_t reg_c,
    const uint16_t shift_value
) {
    registers[reg_c] = registers[reg_b] << shift_value;
}

void CPU::SRL(
    const uint16_t reg_b,
    const uint16_t reg_c,
    const uint16_t shift_value
) {
    registers[reg_c] = (unsigned) registers[reg_b] >> shift_value;
}

void CPU::SLT(
    const uint16_t reg_a,
    const uint16_t reg_b,
    const uint16_t reg_c
) {
    registers[reg_c] =
        static_cast<uint16_t>(registers[reg_a] < registers[reg_b]);
}

*/

#include "cpu.h"
#include "console.h"
#include "memory.h"
#include "gpu.h"
#include "controller.h"
#include <iostream>

int box_x_position = 64;  // Box starts at center
int box_y_position = 64;  // Box starts at center

CPU::CPU(Console* console) : console(console) {}

void CPU::execute(const uint32_t instruction) {
    const uint8_t opcode_shift = 26;
    const uint16_t opcode = instruction >> opcode_shift;
    const uint16_t first_six_bits_mask = 0x3f;

    std::cerr << "Executing Instruction: Opcode = " << std::hex 
              << static_cast<int>(opcode) << std::endl;

    // ✅ Handle movement based on controller input
    if (opcode == static_cast<uint16_t>(Opcode::L8U)) {  
        uint8_t controller_state = console->memory.l8u(0x7000); // Read controller input
        
        // Update box position based on input
        if (controller_state & CONTROLLER_LEFT_MASK)  box_x_position -= 1;
        if (controller_state & CONTROLLER_RIGHT_MASK) box_x_position += 1;
        if (controller_state & CONTROLLER_UP_MASK)    box_y_position -= 1;
        if (controller_state & CONTROLLER_DOWN_MASK)  box_y_position += 1;

        // Ensure box position stays within screen bounds
        if (box_x_position < 0) box_x_position = 0;
        if (box_x_position >= GPU::FRAME_WIDTH) box_x_position = GPU::FRAME_WIDTH - 1;
        if (box_y_position < 0) box_y_position = 0;
        if (box_y_position >= GPU::FRAME_HEIGHT) box_y_position = GPU::FRAME_HEIGHT - 1;

        std::cerr << "Updated Box Position: X = " << box_x_position 
                  << ", Y = " << box_y_position << std::endl;

        // ✅ Store new box position in VRAM
        uint16_t vram_address = GPU::getPixelAddress(box_x_position, box_y_position);
        console->memory.w8u(vram_address, 0xFF);

        // ✅ Force the GPU to render the updated frame
        console->gpu.renderFrame();
    }

    // Continue executing normal instructions
    if (opcode == static_cast<uint16_t>(Opcode::RTYPE)) {
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

// ✅ MISSING FUNCTION IMPLEMENTATIONS (Added Here)

void CPU::L16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    uint32_t effective_address = registers[reg_a] + immediate;
    registers[reg_b] = console->memory.l16u(effective_address);
}

void CPU::S16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    uint16_t effective_address = registers[reg_a] + immediate;
    console->memory.w16u(effective_address, registers[reg_b]);
}

void CPU::BNE(uint16_t reg_a, uint16_t reg_b, uint16_t immediate) {
    if (registers[reg_a] != registers[reg_b]) {
        program_counter += 4 + (4 * immediate);
    } else {
        program_counter += PC_INCREMENT;
    }
}

void CPU::J(uint16_t immediate) {
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

void CPU::XOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] ^ registers[reg_b];
}

void CPU::AND(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = registers[reg_a] & registers[reg_b];
}

void CPU::SLT(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c) {
    registers[reg_c] = (registers[reg_a] < registers[reg_b]) ? 1 : 0;
}

void CPU::SRA(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value) {
    registers[reg_c] = (int16_t)registers[reg_b] >> shift_value;
}

void CPU::SLL(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value) {
    registers[reg_c] = registers[reg_b] << shift_value;
}

void CPU::SRL(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value) {
    registers[reg_c] = (uint16_t)registers[reg_b] >> shift_value;
}