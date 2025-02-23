#ifndef CONSOLE_H
#define CONSOLE_H

#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"

#include <cstdint>

class Console {
public:
    Console();
    virtual ~Console(); // Virtual destructor to ensure cleanup

    void pollInput();
    void stopExecution();
    bool isRunning() const;

private:
    CPU cpu;
    OS os;
    Memory memory;
    GPU gpu;
    uint8_t controllerState;
    bool is_running;
};

#endif // CONSOLE_H