#pragma once

#include "bit_definitions.h"

#include <cstdint>

class Filter {
  public:
    struct Color {
        uint8_t red;
        uint8_t green;
        uint8_t blue;
    };

    void setColor(uint8_t red, uint8_t green, uint8_t blue);
    Color getColor();

  private:
    Color color{BYTE_MASK, 0, 0};
};
