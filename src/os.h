#pragma once

#include "console.h"

#include <string>
#include <iostream>

class Console;

class OS {
    private:
        Console* c;
    public:
        OS(Console* c_);

        void reset(std::string& filename);
        void loop();
};