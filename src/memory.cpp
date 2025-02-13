#include "memory.h"

#include "console.h"
#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

Memory::Memory(Console* console_instance)
    : console_instance(console_instance) {} // initialize console_instance

// Link for code for permission checks found from ChatGPT
// https://chatgpt.com/share/67a42b0d-6d68-800e-b329-a5184489016e

bool Memory::isReadable(const uint32_t ADDRESS) {
    return (ADDRESS < static_cast<uint32_t>(Address::IO_START)) || // RAM
           (ADDRESS >= static_cast<uint32_t>(Address::STACK_START) &&
            ADDRESS < static_cast<uint32_t>(Address::VRAM_START)) || // Stack
           (ADDRESS >= static_cast<uint32_t>(Address::VRAM_START) &&
            ADDRESS < static_cast<uint32_t>(Address::VRAM_END)) || // VRAM
           (ADDRESS == static_cast<uint32_t>(Address::CONTROLLER_DATA)) ||
           (ADDRESS == static_cast<uint32_t>(Address::STDIN)) ||
           (ADDRESS >= static_cast<uint32_t>(Address::SLUG_START) &&
            ADDRESS < static_cast<uint32_t>(Address::ADDRESS_SPACE_END)
           ); // SLUG
}


bool Memory::isWritable(const uint32_t ADDRESS) {
    return (ADDRESS < static_cast<uint32_t>(Address::IO_START)) || // RAM
           (ADDRESS >= static_cast<uint32_t>(Address::STACK_START) &&
            ADDRESS < static_cast<uint32_t>(Address::VRAM_START)) || // Stack
           (ADDRESS >= static_cast<uint32_t>(Address::VRAM_START) &&
            ADDRESS < static_cast<uint32_t>(Address::VRAM_END)) || // VRAM
           (ADDRESS == static_cast<uint32_t>(Address::STDOUT)) ||
           (ADDRESS == static_cast<uint32_t>(Address::STDERR)) ||
           (ADDRESS == static_cast<uint32_t>(Address::STOP_EXECUTION));
}

bool Memory::isExecutable(const uint32_t ADDRESS) {
    return (
        ADDRESS >= static_cast<uint32_t>(Address::SLUG_START) &&
        ADDRESS < static_cast<uint32_t>(Address::ADDRESS_SPACE_END)
    ); // SLUG file
}


uint8_t Memory::l8u(const uint16_t LOAD_ADDRESS) const {
    if (!isReadable(LOAD_ADDRESS)) {
        throw std::invalid_argument(
            "Cannot read from " + std::to_string(LOAD_ADDRESS)
        );
    }

    uint8_t out = 0;
    if (LOAD_ADDRESS == static_cast<uint32_t>(Address::CONTROLLER_DATA)) {
        // TODO: get controller data
    } else if (LOAD_ADDRESS == static_cast<uint32_t>(Address::STDIN)) {
        out = getchar();
    } else {
        out = mem_array[LOAD_ADDRESS];
    }
    return out;
}

uint16_t Memory::l16u(const uint16_t LOAD_ADDRESS) const {
    // checking if the alignment is right
    uint16_t out = 0;
    if ((LOAD_ADDRESS & 1) != 0) {
        // The address is odd and therefore wrong
        std::cerr << "warning trying to read the word on a false word address"
                  << std::endl;
    }
    out = (l8u(LOAD_ADDRESS) << BITS_PER_BYTE) | l8u(LOAD_ADDRESS + 1);
    return out;
}

uint32_t Memory::l32u(const uint16_t LOAD_ADDRESS) const {
    // checking if the alignment is right
    uint32_t out = 0;
    if ((LOAD_ADDRESS & 1) != 0) {
        // The address is odd and therefore wrong
        std::cerr << "warning trying to read the word on a false word address"
                  << std::endl;
    }
    out = (l8u(LOAD_ADDRESS) << BITS_PER_BYTE * 3) |
          (l8u(LOAD_ADDRESS + 1) << BITS_PER_BYTE * 2) |
          (l8u(LOAD_ADDRESS + 2) << BITS_PER_BYTE) | (l8u(LOAD_ADDRESS + 3));
    return out;
}

uint32_t Memory::loadInstruction(const uint16_t LOAD_ADDRESS) const {
    if (!isExecutable(LOAD_ADDRESS)) {
        throw std::invalid_argument(
            std::to_string(LOAD_ADDRESS) + " is not executable"
        );
    }
    return l32u(LOAD_ADDRESS);
}

// got the write code from chat gpt
// https://chatgpt.com/share/67a02e08-1ad0-8013-a682-bbb8496babd0
void Memory::w8u(const uint16_t ADDRESS, const uint8_t VALUE) {
    if (!isWritable(ADDRESS)) {
        throw std::invalid_argument(
            "Cannot write to " + std::to_string(ADDRESS)
        );
    }

    if (ADDRESS == static_cast<uint32_t>(Address::STDOUT)) {
        std::cout << char(VALUE);
    } else if (ADDRESS == static_cast<uint32_t>(Address::STDERR))
        std::cerr << char(VALUE);
    else if (ADDRESS == static_cast<uint32_t>(Address::STOP_EXECUTION)) {
        console_instance->stopExecution();
    } else {
        mem_array[ADDRESS] = VALUE;
    }
}

void Memory::w16u(const uint16_t ADDRESS, const uint16_t VALUE) {
    if ((ADDRESS & 1) != 0) {
        std::cerr << "warning: trying to write the word on an unaligned address"
                  << std::endl;
    }
    const uint8_t LOW_BYTE = 0xff;
    w8u(ADDRESS, (VALUE >> BITS_PER_BYTE) & LOW_BYTE); // High byte
    w8u(ADDRESS + 1, VALUE & LOW_BYTE);                // Low byte
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
    uint16_t const DATA_SIZE = getDataSize();
    uint16_t const LOAD_DATA_ADDRESS = getLoadDataAddress();
    uint16_t const PROGRAM_DATA_ADDRESS = getProgramDataAddress();

    std::copy(
        mem_array.begin() + LOAD_DATA_ADDRESS,
        mem_array.begin() + LOAD_DATA_ADDRESS + DATA_SIZE,
        mem_array.begin() + PROGRAM_DATA_ADDRESS
    );
}

void Memory::loadFile(std::ifstream& file_stream) {
    file_stream.seekg(0, std::ios::end);
    std::streamsize const FILE_SIZE = file_stream.tellg();

    if (FILE_SIZE > static_cast<uint32_t>(Address::SLUG_SIZE)) {
        throw std::runtime_error("ROM file is too large to fit in memory.");
    }
    // Reads only the actual file size to avoid reading past the end of a
    // smaller file.
    file_stream.read(
        reinterpret_cast<char*>(
            mem_array.begin() + static_cast<uint32_t>(Address::SLUG_START)
        ),
        FILE_SIZE
    );
}
