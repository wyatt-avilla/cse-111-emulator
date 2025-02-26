#include <cstdint>

#pragma once

class Console;

class Controller {
    enum class Mask : uint8_t {
        A = 0x80,
        B = 0x40,
        SELECT = 0x20,
        START = 0x10,
        UP = 0x08,
        DOWN = 0x04,
        LEFT = 0x02,
        RIGHT = 0x01,
    };

  public:
    Controller(Console* console);
    [[nodiscard]] static uint8_t getState();

  private:
    Console* console;
};
