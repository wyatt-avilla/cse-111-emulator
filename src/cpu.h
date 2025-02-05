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
    void BEQ();
    void L16();
    void L8U();
    void J();
    void S16();
    void S8();
    void ADDI();
    void BNE();
    void JAL();

    void JAL(uint16_t immediate);

    // R TYPE
    void SUB();
    void OR();
    void NOR();
    void ADD();
    void SRA();
    void XOR();
    void AND();
    void JR();
    void SLL();
    void SRL();
    void SLT();

  private:
    Console* console;

    struct {
        uint16_t reg_a, reg_b, reg_c, shift_value, immediate;
    } instruction_context{};

    uint16_t program_counter;

    static const std::size_t num_registers = 32;
    std::array<uint16_t, num_registers> registers{};

    static const std::size_t jump_table_size = 64;
    std::array<void (CPU::*)(), jump_table_size> jump_table{};

    void executeTypeI(uint32_t instruction);
    void executeTypeR(uint32_t instruction);
};
