#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <string>

class Controller;
class Console;

class Memory {
public:
    Memory(Console* console_instance, Controller* controller_instance);  // Fix: Pass controller reference

    uint8_t l8u(uint16_t load_address) const;
    uint16_t l16u(uint16_t load_address) const;
    uint32_t l32u(uint16_t load_address) const;
    uint32_t loadInstruction(uint16_t load_address) const;
    void w8u(uint16_t address, uint8_t value);
    void w16u(uint16_t address, uint16_t value);

private:
    Console* console_instance;
    Controller* controller;  // Fix: Store reference to controller
};