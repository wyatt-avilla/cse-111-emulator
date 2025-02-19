#include "gpu.h"
#include <iostream>
#include <cstring>

GPU::GPU() {
    std::memset(vram, 0, sizeof(vram));
}

int GPU::getPixelAddress(int x, int y) {
    return x + (y * FRAME_WIDTH);
}

void GPU::setPixel(int x, int y, uint8_t grayLevel) {
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT) return;
    int index = getPixelAddress(x, y);
    vram[index] = grayLevel;
}

void GPU::renderFrame() {
    std::cout << "GPU Rendering Frame (top-left 16x16 block):" << std::endl;
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int index = getPixelAddress(x, y);
            uint8_t pixel = vram[index];
            // Use symbols to represent different gray levels:
            if (pixel < 0x40)
                std::cout << ".";
            else if (pixel < 0x80)
                std::cout << "-";
            else if (pixel < 0xC0)
                std::cout << "*";
            else
                std::cout << "#";
        }
        std::cout << std::endl;
    }
}
