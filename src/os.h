#pragma once

#include "banana_memory.h"
#include "cpu.h"

#include <string>
#include <iostream>

class Console;

class OS {
    private:
        std::string filename;
        BananaMemory* m;
        CPU* cpu;
    public:
        OS(BananaMemory* m_, CPU* cpu_, std::string& filename_);

        void reset();
        void loop();
};