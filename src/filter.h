#pragma once

#include <cstdint>
#include <array>

class Filter {
public:
    // Apply a background color filter to the pixels
    // This will set all pixels to the specified red, green, and blue values
    static void applyBackgroundColorFilter(std::array<unsigned int, 65536>& pixels, uint8_t red, uint8_t green, uint8_t blue);

private:
    // Prevent instantiation of the Filter class
    Filter() = default;
};
