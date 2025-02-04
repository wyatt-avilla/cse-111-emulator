#include "banana_memory.h"

#include "console.h" // change to to the correct name later

#include <fstream> // For file handling
#include <iostream>
#include <string.h> // For the memset of the array

BananaMemory::BananaMemory(Console* console) : console(console) {}

uint8_t BananaMemory::l8u(uint16_t load_address) const {
    uint8_t out = 0;
    if (load_address == 0x70000) {
        // get controller data
    } else if (load_address == 0x7100) {
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
    mem_array[address] = value;
}

void BananaMemory::w16u(uint16_t address, uint16_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write word on an unaligned address"
                  << std::endl;
    }
    mem_array[address] = (value >> 8) & 0xFF; // High byte
    mem_array[address + 1] = value & 0xFF;    // Low byte
}

void BananaMemory::writeInstrcution(uint16_t address, uint32_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write word on an unaligned address"
                  << std::endl;
    }
    mem_array[address] = (value >> 24) & 0xFF; // Highest byte
    mem_array[address + 1] = (value >> 16) & 0xFF;
    mem_array[address + 2] = (value >> 8) & 0xFF;
    mem_array[address + 3] = value & 0xFF; // Lowest byte
}
