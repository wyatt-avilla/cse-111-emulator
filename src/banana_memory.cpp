#include "banana_memory.h"

#include "console.h"

#include <cstdio>
#include <iostream>

BananaMemory::BananaMemory(Console* console) : console(console) {}

uint8_t BananaMemory::l8u(uint16_t load_address) const {
    uint8_t out = 0;
    if (load_address == controller_data_address) {
        // TODO: get controller data
    } else if (load_address == stdin_address) {
        out = getchar();
    } else {
        out = mem_array[load_address];
    }
    return out;
}

uint16_t BananaMemory::l16u(uint16_t load_address) const {
    // checking if the alignment is right
    uint16_t out = 0;
    if (load_address & 1) {
        // the adress is odd and there fore wrong
        std::cerr << "warning trying to read word on a false word adress"
                  << std::endl;
    }
    out = (l8u(load_address) << 8) | l8u(load_address + 1);
    return out;
}

uint32_t BananaMemory::loadInstruction(uint16_t load_address) const {
    // checking if the alignment is right
    uint32_t out = 0;
    if (load_address & 1) {
        // the adress is odd and there fore wrong
        std::cerr << "warning trying to read word on a false word adress"
                  << std::endl;
    }
    out = (l8u(load_address) << 24) | (l8u(load_address + 1) << 16) |
          (l8u(load_address + 2) << 8) | (l8u(load_address + 3));
    return out;
}

// got the write code from chat gpt
// https://chatgpt.com/share/67a02e08-1ad0-8013-a682-bbb8496babd0

void BananaMemory::w8u(uint16_t address, uint8_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write word on an unaligned address"
                  << std::endl;
    }
    if (address == stdout_address) {
        std::cout << char(value);
    } else if (address == stderr_address)
        std::cerr << char(value);
    else if (address == stop_execution_address) {
        throw std::runtime_error("stop execution");
    } else {
        mem_array[address] = value;
    }
}

void BananaMemory::w16u(uint16_t address, uint16_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write word on an unaligned address"
                  << std::endl;
    }
    w8u(address, (value >> 8) & 0xFF); // High byte
    w8u(address + 1, value & 0xFF);    // Low byte
}

void BananaMemory::writeInstrcution(uint16_t address, uint32_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write word on an unaligned address"
                  << std::endl;
    }
    w8u(address, (value >> 24) & 0xFF); // Highest byte
    w8u(address + 1, (value >> 16) & 0xFF);
    w8u(address + 2, (value >> 8) & 0xFF);
    w8u(address + 3, value & 0xFF); // Lowest byte
}

uint16_t BananaMemory::getSetupAddress() const { return l16u(0x81e0 + 2); }

uint16_t BananaMemory::getLoopAddress() const { return l16u(0x81e4 + 2); }
