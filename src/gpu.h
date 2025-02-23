#pragma once

#include <SDL2/SDL.h>
#include <cstdint>

class GPU {
  public:
    // Display resolution: 128 x 128 pixels (16,384 bytes)
    static const uint32_t FRAME_WIDTH = 128;
    static const uint32_t FRAME_HEIGHT = 128;
    static const uint32_t VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT; // 16384 bytes

    GPU();
    ~GPU();

    // Given pixel coordinates (x, y), returns the full memory address in VRAM.
    // Calculation: 0x3000 + (x + y * 128)
    uint32_t getPixelAddress(uint32_t x, uint32_t y) const;

    // Optionally set a pixel in the internal frame buffer.
    void setPixel(uint32_t x, uint32_t y, uint8_t grayLevel);

    // Render the current frame: copy external VRAM into the internal buffer,
    // convert grayscale bytes to 32-bit ARGB pixels, and render via SDL.
    void renderFrame();

    // Set the external VRAM pointer (should point to main memory at offset
    // 0x3000).
    void setExternalVRAM(uint8_t* ptr);

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    // Internal VRAM buffer used for rendering (each pixel is 1 byte: grayscale)
    uint8_t vram[VRAM_SIZE];

    // Pointer to the external VRAM region (main memory, starting at 0x3000)
    uint8_t* externalVRAM = nullptr;
};

#endif // GPU_H