#include <iostream>
#include <vector>
#include <stdexcept>
#include <cstdint>
// got this code from chat gpt
//https://chatgpt.com/share/679d2b79-d0c8-8013-a5c4-09fb80077da6

class RegisterFile {
private:
    std::vector<uint16_t> registers;

public:
    RegisterFile(size_t numRegisters) : registers(numRegisters, 0) {}

    uint16_t read(size_t reg) const {
        if (reg >= registers.size()) {
            throw std::out_of_range("Invalid register index");
        }
        return registers[reg];
    }

    void write(size_t reg, uint16_t value) {
        if (reg >= registers.size()) {
            throw std::out_of_range("Invalid register index");
        }
        registers[reg] = value;
    }

    void display() const {
        for (size_t i = 0; i < registers.size(); ++i) {
            std::cout << "R" << i << ": " << registers[i] << "\n";
        }
    }
};