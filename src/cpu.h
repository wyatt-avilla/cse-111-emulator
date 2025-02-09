#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

class Console;

class CPU {
  public:
    enum class Opcode : uint16_t {
        BEQ = 0,
        L16 = 2,
        L8U = 16,
        J = 36,
        S16 = 48,
        S8 = 50,
        ADDI = 54,
        BNE = 59,
        JAL = 61,

        SUB = 0,
        OR = 4,
        NOR = 7,
        ADD = 9,
        SRA = 11,
        XOR = 19,
        AND = 24,
        JR = 28,
        SLL = 32,
        SRL = 35,
        SLT = 36,

        RTYPE = 62,
    };

    CPU(Console* console);

    void execute(uint32_t instruction);

    void set_program_counter_to(uint16_t counter_value);

    uint16_t get_program_counter();

    void set_stack_pointer_to(uint16_t pointer_value);

    // I TYPE
    void BEQ(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void L16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void L8U(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void J(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void S16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void S8(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void ADDI(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void BNE(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void JAL(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);

    void JAL(uint16_t immediate);

    // R TYPE
    void
    SUB(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    OR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    NOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    ADD(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    SRA(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    XOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    AND(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    JR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    SLL(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    SRL(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void
    SLT(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);

  private:
    Console* console;

    uint16_t program_counter;

    static const std::size_t num_registers = 32;
    std::array<uint16_t, num_registers> registers{};

    static const std::size_t jump_table_size = 64;
    std::array<void (CPU::*)(uint16_t, uint16_t, uint16_t), jump_table_size>
        i_type_jump_table{};
    std::array<
        void (CPU::*)(uint16_t, uint16_t, uint16_t, uint16_t),
        jump_table_size>
        r_type_jump_table{};

    void executeTypeI(uint32_t instruction);
    void executeTypeR(uint32_t instruction);
};
