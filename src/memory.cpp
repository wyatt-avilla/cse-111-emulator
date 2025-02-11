#include "memory.h"

#include "console.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

Memory::Memory(Console* console) : console(console) {}
// Link for code for permission checks found from ChatGPT
// https://chatgpt.com/share/67a42b0d-6d68-800e-b329-a5184489016e

bool Memory::isReadable(uint32_t address) const {
    return (address < static_cast<uint32_t>(Address::IO_START)) || // RAM
           (address >= static_cast<uint32_t>(Address::STACK_START) &&
            address < static_cast<uint32_t>(Address::VRAM_START)) || // Stack
           (address >= static_cast<uint32_t>(Address::VRAM_START) &&
            address < static_cast<uint32_t>(Address::VRAM_END)) || // VRAM
           (address == static_cast<uint32_t>(Address::CONTROLLER_DATA)) ||
           (address == static_cast<uint32_t>(Address::STDIN)) ||
           (address >= static_cast<uint32_t>(Address::SLUG_START) &&
            address < static_cast<uint32_t>(Address::ADDRESS_SPACE_END)
           ); // SLUG
}


bool Memory::isWritable(uint32_t address) const {
    return (address < static_cast<uint32_t>(Address::IO_START)) || // RAM
           (address >= static_cast<uint32_t>(Address::STACK_START) &&
            address < static_cast<uint32_t>(Address::VRAM_START)) || // Stack
           (address >= static_cast<uint32_t>(Address::VRAM_START) &&
            address < static_cast<uint32_t>(Address::VRAM_END)) || // VRAM
           (address == static_cast<uint32_t>(Address::STDOUT)) ||
           (address == static_cast<uint32_t>(Address::STDERR)) ||
           (address == static_cast<uint32_t>(Address::STOP_EXECUTION));
}

bool Memory::isExecutable(uint32_t address) const {
    return (
        address >= static_cast<uint32_t>(Address::SLUG_START) &&
        address < static_cast<uint32_t>(Address::ADDRESS_SPACE_END)
    ); // SLUG file
}


uint8_t Memory::l8u(uint16_t load_address) const {
    if (!isReadable(load_address)) {
        throw std::invalid_argument(
            "Cannot read from " + std::to_string(load_address)
        );
    }

    uint8_t out = 0;
    if (load_address == static_cast<uint32_t>(Address::CONTROLLER_DATA)) {
        // TODO: get controller data
    } else if (load_address == static_cast<uint32_t>(Address::STDIN)) {
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

uint32_t Memory::l32u(uint16_t load_address) const {
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

uint32_t Memory::loadInstruction(uint16_t load_address) const {
    if (!isExecutable(load_address)) {
        throw std::invalid_argument(
            std::to_string(load_address) + " is not executable"
        );
    }
    return l32u(load_address);
}

// got the write code from chat gpt
// https://chatgpt.com/share/67a02e08-1ad0-8013-a682-bbb8496babd0
void Memory::w8u(uint16_t address, uint8_t value) {
    if (!isWritable(address)) {
        throw std::invalid_argument(
            "Cannot write to " + std::to_string(address)
        );
    }

    if (address == static_cast<uint32_t>(Address::STDOUT)) {
        std::cout << char(value);
    } else if (address == static_cast<uint32_t>(Address::STDERR))
        std::cerr << char(value);
    else if (address == static_cast<uint32_t>(Address::STOP_EXECUTION)) {
        exit(0);
    } else {
        mem_array[address] = value;
    }
}

void Memory::w16u(uint16_t address, uint16_t value) {
    if (address & 1) {
        std::cerr << "warning: trying to write the word on an unaligned address"
                  << std::endl;
    }
    w8u(address, (value >> 8) & 0xFF); // High byte
    w8u(address + 1, value & 0xFF);    // Low byte
}

uint16_t Memory::getSetupAddress() const {
    return l16u(static_cast<uint32_t>(Address::SETUP) + 2);
}

uint16_t Memory::getLoopAddress() const {
    return l16u(static_cast<uint32_t>(Address::LOOP) + 2);
}

uint16_t Memory::getLoadDataAddress() const {
    return l16u(static_cast<uint32_t>(Address::LOAD_DATA) + 2);
}

uint16_t Memory::getProgramDataAddress() const {
    return l16u(static_cast<uint32_t>(Address::PROGRAM_DATA) + 2);
}

uint16_t Memory::getDataSize() const {
    return l16u(static_cast<uint32_t>(Address::DATA_SIZE) + 2);
}

void Memory::clearRAM() {
    std::fill(
        mem_array.begin(),
        mem_array.begin() + static_cast<uint32_t>(Address::IO_START),
        0
    );
}

void Memory::copyDataSectionToRam() {
    uint16_t data_size = getDataSize();
    uint16_t load_data_address = getLoadDataAddress();
    uint16_t program_data_address = getProgramDataAddress();

    std::copy(
        mem_array.begin() + load_data_address,
        mem_array.begin() + load_data_address + data_size,
        mem_array.begin() + program_data_address
    );
}

void Memory::loadFile(std::ifstream& file_stream) {
    file_stream.seekg(0, std::ios::beg);
    file_stream.read(
        (char*) mem_array.begin() + static_cast<uint32_t>(Address::SLUG_START),
        static_cast<uint32_t>(Address::SLUG_SIZE)
    );
}
