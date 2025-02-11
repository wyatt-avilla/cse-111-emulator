#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <variant>

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

    void setProgramCounterTo(uint16_t counter_value);

    [[nodiscard]] uint16_t getProgramCounter() const;

    void setStackPointerTo(uint16_t pointer_value);

    // I TYPE
    void BEQ(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void L16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void L8U(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void S16(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void BNE(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void S8(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void ADDI(uint16_t reg_a, uint16_t reg_b, uint16_t immediate);
    void J(uint16_t immediate);
    void JAL(uint16_t immediate);

    // R TYPE
    void SUB(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void NOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void ADD(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void XOR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void AND(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void SLT(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void OR(uint16_t reg_a, uint16_t reg_b, uint16_t reg_c);
    void JR(uint16_t reg_a);
    void SLL(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void SRL(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);
    void SRA(uint16_t reg_b, uint16_t reg_c, uint16_t shift_value);

  private:
    Console* console;

    uint16_t program_counter;

    static const std::size_t NUM_REGISTERS = 32;
    std::array<uint16_t, NUM_REGISTERS> registers{};

    static const std::size_t JUMP_TABLE_SIZE = 64;

    // variants and constexprs by claude
    // https://claude.site/artifacts/a672ce20-f93d-44fe-b36c-49a426778c92
    struct RegularIType {
        void (CPU::*func)(uint16_t, uint16_t, uint16_t);
    };

    struct OnlyImmediateIType {
        void (CPU::*func)(uint16_t);
    };

    struct RegularRType {
        void (CPU::*func)(uint16_t, uint16_t, uint16_t);
    };

    struct ShiftRType {
        void (CPU::*func)(uint16_t, uint16_t, uint16_t);
    };

    struct JumpRegisterRType {
        void (CPU::*func)(uint16_t);
    };

    using ITypeVariant = std::variant<RegularIType, OnlyImmediateIType>;
    using RTypeVariant =
        std::variant<RegularRType, JumpRegisterRType, ShiftRType>;


    static constexpr std::array<ITypeVariant, JUMP_TABLE_SIZE>
    createITypeTable() {
        std::array<ITypeVariant, JUMP_TABLE_SIZE> table{};

        auto set_regular = [&table](Opcode op, auto func_ptr) {
            table[static_cast<uint16_t>(op)] = RegularIType{func_ptr};
        };

        auto set_immediate = [&table](Opcode op, auto func_ptr) {
            table[static_cast<uint16_t>(op)] = OnlyImmediateIType{func_ptr};
        };

        set_regular(Opcode::BEQ, &CPU::BEQ);
        set_regular(Opcode::L16, &CPU::L16);
        set_regular(Opcode::L8U, &CPU::L8U);
        set_regular(Opcode::S16, &CPU::S16);
        set_regular(Opcode::S8, &CPU::S8);
        set_regular(Opcode::ADDI, &CPU::ADDI);
        set_regular(Opcode::BNE, &CPU::BNE);

        set_immediate(Opcode::J, &CPU::J);
        set_immediate(Opcode::JAL, &CPU::JAL);

        return table;
    }

    static constexpr std::array<RTypeVariant, JUMP_TABLE_SIZE>
    createRTypeTable() {
        std::array<RTypeVariant, JUMP_TABLE_SIZE> table{};

        auto set_regular = [&table](Opcode op, auto func_ptr) {
            table[static_cast<uint16_t>(op)] = RegularRType{func_ptr};
        };

        auto set_shift = [&table](Opcode op, auto func_ptr) {
            table[static_cast<uint16_t>(op)] = ShiftRType{func_ptr};
        };

        auto set_jump = [&table](Opcode op, auto func_ptr) {
            table[static_cast<uint16_t>(op)] = JumpRegisterRType{func_ptr};
        };

        set_regular(Opcode::SUB, &CPU::SUB);
        set_regular(Opcode::OR, &CPU::OR);
        set_regular(Opcode::NOR, &CPU::NOR);
        set_regular(Opcode::ADD, &CPU::ADD);
        set_regular(Opcode::XOR, &CPU::XOR);
        set_regular(Opcode::AND, &CPU::AND);
        set_regular(Opcode::SLT, &CPU::SLT);

        set_shift(Opcode::SRA, &CPU::SRA);
        set_shift(Opcode::SLL, &CPU::SLL);
        set_shift(Opcode::SRL, &CPU::SRL);

        set_jump(Opcode::JR, &CPU::JR);

        return table;
    }

    const std::array<ITypeVariant, JUMP_TABLE_SIZE> I_TYPE_JUMP_TABLE =
        createITypeTable();
    const std::array<RTypeVariant, JUMP_TABLE_SIZE> R_TYPE_JUMP_TABLE =
        createRTypeTable();


    template <class... Ts> struct Overloaded : Ts... {
        using Ts::operator()...;
    };
    template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

    void executeTypeI(uint32_t instruction);
    void executeTypeR(uint32_t instruction);
};
