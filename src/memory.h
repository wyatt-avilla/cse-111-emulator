#include <array>
#include <cstdint>
#include <fstream>

#pragma once

class Console;

class Memory {
  public:
    Memory(Console* console_instance);

    enum class Address {
        STDIN = 0x7100,
        STDOUT = 0x7110,
        STDERR = 0x7120,
        STOP_EXECUTION = 0x7200,
        CONTROLLER_DATA = 0x7000,
        IO_START = 0x7000,
        STACK_START = 0x1000,
        STACK_END = 0x3000,
        VRAM_START = 0x3000,
        VRAM_END = 0x4000,
        SLUG_START = 0x8000,
        SLUG_SIZE = 0x8000,
        SETUP = 0x81e0,
        LOOP = 0x81e4,
        LOAD_DATA = 0x81e8,
        PROGRAM_DATA = 0x81ec,
        DATA_SIZE = 0x81f0,
        ADDRESS_SPACE_END = 0x10000,
    };


    [[nodiscard]] uint8_t l8u(uint16_t load_address) const;
    [[nodiscard]] uint16_t l16u(uint16_t load_address) const;
    [[nodiscard]] uint32_t l32u(uint16_t load_address) const;
    [[nodiscard]] uint32_t loadInstruction(uint16_t load_address) const;
    void w8u(uint16_t address, uint8_t value);
    void w16u(uint16_t address, uint16_t value);

    [[nodiscard]] uint16_t getSetupAddress() const;
    [[nodiscard]] uint16_t getLoopAddress() const;
    [[nodiscard]] uint16_t getLoadDataAddress() const;
    [[nodiscard]] uint16_t getProgramDataAddress() const;
    [[nodiscard]] uint16_t getDataSize() const;

    void clearRAM();
    void copyDataSectionToRam();
    void loadFile(std::ifstream& file_stream);

  private:
    Console* console_instance; 
    const uint8_t BITS_PER_BYTE = 8;

    std::array<
        uint8_t,
        static_cast<uint32_t>(Memory::Address::ADDRESS_SPACE_END)>
        mem_array{};
    [[nodiscard]] static bool isReadable(uint32_t address);
    [[nodiscard]] static bool isWritable(uint32_t address);
    [[nodiscard]] static bool isExecutable(uint32_t address);
};
