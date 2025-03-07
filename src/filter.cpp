#include "filter.h"
#include <cstddef>  // Include this header to use size_t

void Filter::applyBackgroundColorFilter(std::array<uint32_t, 65536>& pixels, uint8_t red, uint8_t green, uint8_t blue) {
    // Loop through each pixel and apply the background color filter
    for (size_t i = 0; i < pixels.size(); ++i) {
        pixels[i] = (static_cast<uint32_t>(red) << 16) | (static_cast<uint32_t>(green) << 8) | static_cast<uint32_t>(blue);
    }
}
