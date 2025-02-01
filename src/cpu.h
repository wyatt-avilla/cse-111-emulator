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
    uint16_t* registers;
    void (CPU::* jump_table[])();
};
