#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#define IO_START 0x7000
#define STK_START 0x1000
#define STK_END 0x3000
#define VRAM_START 0x3000
#define VRAM_END 0x4000
#define SLUG_START 0x8000
#define SLUG_SIZE 0x8000
#define MEM_SIZE 0x10000
#define SETUP_ADDRESS 0x81e0
#define LOOP_ADDRESS 0x81e4

class Console;

class Memory {
  private:
    Console* console;
    uint8_t mem_array[MEM_SIZE]; // the size of the array full address space for
                                 // the banana
    bool isReadable(uint32_t address) const;
    bool isWritable(uint32_t address) const;
    bool isExecutable(uint32_t address) const;

  public:
    Memory(Console* console);

    const uint16_t stdin_address = 0x7100;
    const uint16_t stdout_address = 0x7110;
    const uint16_t stderr_address = 0x7120;
    const uint16_t stop_execution_address = 0x7200;
    const uint16_t controller_data_address = 0x7000;

    uint8_t l8u(uint16_t load_address) const;
    uint16_t l16u(uint16_t load_address) const;
    uint32_t l32u(uint16_t load_address) const;
    uint32_t loadInstruction(uint16_t load_address) const;
    void w8u(uint16_t address, uint8_t value);
    void w16u(uint16_t address, uint16_t value);

    uint16_t getSetupAddress() const;
    uint16_t getLoopAddress() const;
    uint16_t getLoadDataAddress() const;
    uint16_t getProgramDataAddress() const;
    uint16_t getDataSize() const;

    void clearRAM();
    void loadFile(std::ifstream& file_stream);
};
