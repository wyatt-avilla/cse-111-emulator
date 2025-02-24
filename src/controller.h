#pragma once
#include "console.h"
#include <cstdint>
#include <iostream>

// Controller Button Masks
#define CONTROLLER_A_MASK      ((uint8_t)0x80)
#define CONTROLLER_B_MASK      ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)
#define CONTROLLER_START_MASK  ((uint8_t)0x10)
#define CONTROLLER_UP_MASK     ((uint8_t)0x08)
#define CONTROLLER_DOWN_MASK   ((uint8_t)0x04)
#define CONTROLLER_LEFT_MASK   ((uint8_t)0x02)
#define CONTROLLER_RIGHT_MASK  ((uint8_t)0x01)

// Memory-Mapped IO Addresses
#define IO_CONTROLLER_DATA   0x7000
#define IO_DEBUG_STDIN       0x7100
#define IO_DEBUG_STDOUT      0x7110
#define IO_DEBUG_STDERR      0x7120
#define IO_STOP_EXECUTION    0x7200

class Console;

class Controller {
  public:
    explicit Controller(Console* console);

    // Update and display controller state
    void update();

    // Display controller state in binary and named format
    void displayState() const;

    // Debug IO functions (read stdin, write stdout/stderr, stop execution)
    uint8_t readStdin();
    void writeStdout(uint8_t value);
    void writeStderr(uint8_t value);
    void stopExecution();

  private:
    Console* console;
    uint8_t controller_state{0}; // Stores current controller state (8-bit)
};
