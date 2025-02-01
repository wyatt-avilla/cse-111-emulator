#include <cstdint>

class CPU {
  public:
    CPU();

    void execute(uint16_t instruction);

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
    uint16_t program_counter;

    static const std::size_t num_registers = 32;
    std::array<uint16_t, num_registers> registers{};

    static const std::size_t jump_table_size = 64;
    std::array<void (CPU::*)(), jump_table_size> jump_table{};

    void executeTypeI(uint16_t instruction);
    void executeTypeR(uint16_t instruction);
};
