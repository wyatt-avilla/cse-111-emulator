#pragma once

#include "console.h"

#include <string>
#include <iostream>

class Console;

class OS {
    private:
        std::string filename;
        Console* c;
    public:
        OS(Console* c_, std::string& filename_);

        void reset();
        void loop();
};