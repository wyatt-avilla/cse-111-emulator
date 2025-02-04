#pragma once

#include <memory>
#include <string>


class Console; // change to the actual name of the console 

class BananaMemory {
    private:
        Console * console;
        uint8_t mem_array[0X10000]; // the size of array full adress sapce for the banana
    public:
        BananaMemory(Console * console);
        uint8_t l8u(uint16_t load_address) const;
        uint16_t l16u(uint16_t load_address) const;
        uint32_t loadInstruction(uint16_t load_address) const;
        void w8u(uint16_t address, uint8_t value);
        void w16u(uint16_t address, uint16_t value);
        void writeInstrcution(uint16_t address, uint32_t value);
};
