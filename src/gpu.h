#pragma once
#ifndef GPU_H
#define GPU_H

#include <cstdint>

class GPU {
public:
    static const int FRAME_WIDTH = 128;
    static const int FRAME_HEIGHT = 128;
    static const int VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT; // 16384 bytes

    uint8_t vram[VRAM_SIZE];

    GPU();

    // Returns the index in VRAM for pixel (x, y)
    int getPixelAddress(int x, int y);

    // Sets the pixel at (x, y) to the given gray level (0x00 = Black, 0xff = White)
    void setPixel(int x, int y, uint8_t grayLevel);

    // Renders the frame; for demonstration, prints a 16x16 block of pixels
    void renderFrame();
};

#endif // GPU_H
