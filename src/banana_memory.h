#pragma once

#include <memory>
#include <string>


class Console;

class BananaMemory {
  private:
    Console* console;
    uint8_t mem_array[0X10000]; // the size of array full adress sapce for the
                                // banana
    bool isReadable(uint16_t address) const;
    bool isWritable(uint16_t address) const;
    bool isExecutable(uint16_t address) const;

  public:
    BananaMemory(Console* console);

    const uint16_t stdin_address = 0x7100;
    const uint16_t stdout_address = 0x7110;
    const uint16_t stderr_address = 0x7120;
    const uint16_t stop_execution_address = 0x7200;
    const uint16_t controller_data_address = 0x7000;

    uint8_t l8u(uint16_t load_address) const;
    uint16_t l16u(uint16_t load_address) const;
    uint32_t loadInstruction(uint16_t load_address) const;
    void w8u(uint16_t address, uint8_t value);
    void w16u(uint16_t address, uint16_t value);
    void writeInstrcution(uint16_t address, uint32_t value);

    uint16_t getSetupAddress() const;
    uint16_t getLoopAddress() const;
};
