#include "memory.h"

#include "console.h"

#include <cstdio>
#include <cstring>
#include <iostream>

Memory::Memory(Console* console) : console(console) {}
// Link for code for permission checks found from ChatGPT
// https://chatgpt.com/share/67a42b0d-6d68-800e-b329-a5184489016e

bool Memory::isReadable(uint16_t address) const {
    return (address >= RAM_START && address < IO_START) ||   // RAM
           (address >= STACK_START && address < VRAM_START) || // Stack
           (address >= VRAM_START && address < VRAM_END) ||  // VRAM
           (address == controller_data_address) || (address == stdin_address) ||
           (address >= SLUG_START && address < SLUG_END); // SLUG
}


bool Memory::isWritable(uint16_t address) const {
    return (address >= RAM_START && address < IO_START) ||   // RAM
           (address >= STACK_START && address < VRAM_START) || // Stack
           (address >= VRAM_START && address < VRAM_END) ||  // VRAM
           (address == stdout_address) || (address == stderr_address) ||
           (address == stop_execution_address);
}

bool Memory::isExecutable(uint16_t address) const {
    return (address >= SLUG_START && address < SLUG_END); // SLUG file
}


uint8_t Memory::l8u(uint16_t load_address) const {
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

uint16_t Memory::l16u(uint16_t load_address) const {
    // checking if the alignment is right
    uint16_t out = 0;
    if (load_address & 1) {
        // The address is odd and therefore wrong
        std::cerr << "warning trying to read the word on a false word address"
                  << std::endl;
    }
    out = (l8u(load_address) << 8) | l8u(load_address + 1);
    return out;
}

uint32_t Memory::loadInstruction(uint16_t load_address) const {
    // checking if the alignment is right
    uint32_t out = 0;
    if (load_address & 1) {
        // The address is odd and therefore wrong
        std::cerr << "warning trying to read the word on a false word address"
                  << std::endl;
    }
    out = (l8u(load_address) << 24) | (l8u(load_address + 1) << 16) |
          (l8u(load_address + 2) << 8) | (l8u(load_address + 3));
    return out;
}

// got the write code from chat gpt
// https://chatgpt.com/share/67a02e08-1ad0-8013-a682-bbb8496babd0
void Memory::w8u(uint16_t address, uint8_t value) {
    if (address == stdout_address) {
        std::cout << char(value);
    } else if (address == stderr_address)
        std::cerr << char(value);
    else if (address == stop_execution_address) {
        exit(0);
    } else {
        mem_array[address] = value;
    }
}

bool Memory::isStopAddressWritten() const {
    constexpr uint16_t STOP_ADDRESS = 0xFFFF; // Replace with actual stop address
    return read(STOP_ADDRESS) != 0;  // Assuming nonzero value indicates writing occurred
}

void Memory::w16u(uint16_t address, uint16_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write the word on an unaligned address"
                  << std::endl;
    }
    w8u(address, (value >> 8) & 0xFF); // High byte
    w8u(address + 1, value & 0xFF);    // Low byte
}

void Memory::writeInstrcution(uint16_t address, uint32_t value) {
    if (address & 3) {
        std::cerr << "warning: trying to write the word on an unaligned address"
                  << std::endl;
    }
    w8u(address, (value >> 24) & 0xFF); // Highest byte
    w8u(address + 1, (value >> 16) & 0xFF);
    w8u(address + 2, (value >> 8) & 0xFF);
    w8u(address + 3, value & 0xFF); // Lowest byte
}

uint16_t Memory::getSetupAddress() const { return l16u(SETUP_ADDRESS + 2); }

uint16_t Memory::getLoopAddress() const { return l16u(LOOP_ADDRESS + 2); }

void Memory::clearRAM() { memset(mem_array, 0, IO_START); }
