#include "memory.h"

#include "bit_definitions.h"
#include "console.h"
#include "controller.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>

Memory::Memory(Console* console_instance)
    : console_instance(console_instance) {}

bool Memory::isReadable(const uint32_t address) {
    return (address <=
            static_cast<std::underlying_type_t<Address>>(Address::IO_START)) ||
           (address >= static_cast<std::underlying_type_t<Address>>(
                           Address::STACK_START
                       ) &&
            address <
                static_cast<std::underlying_type_t<Address>>(Address::VRAM_START
                )) ||
           (address >=
                static_cast<std::underlying_type_t<Address>>(Address::VRAM_START
                ) &&
            address <
                static_cast<std::underlying_type_t<Address>>(Address::VRAM_END)
           ) ||
           (address == static_cast<std::underlying_type_t<Address>>(
                           Address::CONTROLLER_DATA
                       )) ||
           (address ==
            static_cast<std::underlying_type_t<Address>>(Address::STDIN)) ||
           (address >=
                static_cast<std::underlying_type_t<Address>>(Address::SLUG_START
                ) &&
            address < static_cast<std::underlying_type_t<Address>>(
                          Address::ADDRESS_SPACE_END
                      ));
}


bool Memory::isWritable(const uint32_t address) {
    return (address <=
            static_cast<std::underlying_type_t<Address>>(Address::IO_START)) ||
           (address >= static_cast<std::underlying_type_t<Address>>(
                           Address::STACK_START
                       ) &&
            address <
                static_cast<std::underlying_type_t<Address>>(Address::VRAM_START
                )) ||
           (address >=
                static_cast<std::underlying_type_t<Address>>(Address::VRAM_START
                ) &&
            address <
                static_cast<std::underlying_type_t<Address>>(Address::VRAM_END)
           ) ||
           (address ==
            static_cast<std::underlying_type_t<Address>>(Address::STDOUT)) ||
           (address ==
            static_cast<std::underlying_type_t<Address>>(Address::STDERR)) ||
           (address ==
            static_cast<std::underlying_type_t<Address>>(Address::STOP_EXECUTION
            ));
}

bool Memory::isExecutable(const uint32_t address) {
    return (
        address >=
            static_cast<std::underlying_type_t<Address>>(Address::SLUG_START) &&
        address < static_cast<std::underlying_type_t<Address>>(
                      Address::ADDRESS_SPACE_END
                  )
    );
}

uint8_t Memory::l8u(const uint16_t load_address) const {
    if (!isReadable(load_address)) {
        throw std::invalid_argument(
            "Cannot read from " + std::to_string(load_address)
        );
    }

    uint8_t out = 0;
    if (load_address ==
        static_cast<std::underlying_type_t<Address>>(Address::CONTROLLER_DATA
        )) {
        out = Controller::getState();
    } else if (load_address ==
               static_cast<std::underlying_type_t<Address>>(Address::STDIN)) {
        out = getchar();
    } else {
        out = mem_array[load_address];
    }
    return out;
}

uint16_t Memory::l16u(const uint16_t load_address) const {
    uint16_t out = 0;
    if ((load_address & 1) != 0) {
        std::cerr << "warning trying to read the word on a false word address"
                  << std::endl;
    }
    out = (l8u(load_address) << BITS_PER_BYTE) | l8u(load_address + 1);
    return out;
}

uint32_t Memory::l32u(const uint16_t load_address) const {
    uint32_t out = 0;
    if ((load_address & 1) != 0) {
        std::cerr << "warning trying to read the word on a false word address"
                  << std::endl;
    }
    out = (l8u(load_address) << BITS_PER_BYTE * 3) |
          (l8u(load_address + 1) << BITS_PER_BYTE * 2) |
          (l8u(load_address + 2) << BITS_PER_BYTE) | (l8u(load_address + 3));
    return out;
}

uint32_t Memory::loadInstruction(const uint16_t load_address) const {
    if (!isExecutable(load_address)) {
        throw std::invalid_argument(
            std::to_string(load_address) + " is not executable"
        );
    }
    return l32u(load_address);
}

void Memory::w8u(const uint16_t address, const uint8_t value) {
    if (!isWritable(address)) {
        throw std::invalid_argument(
            "Cannot write to " + std::to_string(address)
        );
    }

    if (address ==
        static_cast<std::underlying_type_t<Address>>(Address::STDOUT)) {
        std::cout << static_cast<char>(value);
    } else if (address ==
               static_cast<std::underlying_type_t<Address>>(Address::STDERR)) {
        std::cerr << static_cast<char>(value);
    } else if (address == static_cast<std::underlying_type_t<Address>>(
                              Address::STOP_EXECUTION
                          )) {
        console_instance->stopExecution();
    } else {
        mem_array[address] = value;
    }
}

void Memory::w16u(const uint16_t address, const uint16_t value) {
    if ((address & 1) != 0) {
        std::cerr << "warning: trying to write the word on an unaligned address"
                  << std::endl;
    }
    w8u(address, (value >> BITS_PER_BYTE) & BYTE_MASK);
    w8u(address + 1, value & BYTE_MASK);
}

uint16_t Memory::getSetupAddress() const {
    return l16u(
        static_cast<std::underlying_type_t<Address>>(Address::SETUP) + 2
    );
}

uint16_t Memory::getLoopAddress() const {
    return l16u(
        static_cast<std::underlying_type_t<Address>>(Address::LOOP) + 2
    );
}

uint16_t Memory::getLoadDataAddress() const {
    return l16u(
        static_cast<std::underlying_type_t<Address>>(Address::LOAD_DATA) + 2
    );
}

uint16_t Memory::getProgramDataAddress() const {
    return l16u(
        static_cast<std::underlying_type_t<Address>>(Address::PROGRAM_DATA) + 2
    );
}

uint16_t Memory::getDataSize() const {
    return l16u(
        static_cast<std::underlying_type_t<Address>>(Address::DATA_SIZE) + 2
    );
}

void Memory::clearRAM() {
    std::fill(
        mem_array.begin(),
        mem_array.begin() +
            static_cast<std::underlying_type_t<Address>>(Address::IO_START),
        0
    );
}

void Memory::copyDataSectionToRam() {
    uint16_t const data_size = getDataSize();
    uint16_t const load_data_address = getLoadDataAddress();
    uint16_t const program_data_address = getProgramDataAddress();

    std::copy(
        mem_array.begin() + load_data_address,
        mem_array.begin() + load_data_address + data_size,
        mem_array.begin() + program_data_address
    );
}

void Memory::loadFile(std::ifstream& file_stream) {
    file_stream.seekg(0, std::ios::end);
    std::streamsize const file_size = file_stream.tellg();
    file_stream.seekg(0, std::ios::beg);

    if (file_size >
        static_cast<std::underlying_type_t<Address>>(Address::SLUG_SIZE)) {
        throw std::runtime_error("ROM file is too large to fit in memory.");
    }

    file_stream.read(
        reinterpret_cast<char*>(
            mem_array.begin() +
            static_cast<std::underlying_type_t<Address>>(Address::SLUG_START)
        ),
        file_size
    );
}