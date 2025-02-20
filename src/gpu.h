#ifndef GPU_H
#define GPU_H

#include <cstdint>
#include <SDL2/SDL.h>

class GPU {
public:
    static const int FRAME_WIDTH = 128;
    static const int FRAME_HEIGHT = 128;
    static const int VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT;

    uint8_t vram[VRAM_SIZE];

    GPU();
    ~GPU();

    // Returns the index in VRAM for pixel (x, y)
    int getPixelAddress(int x, int y);

    // Sets the pixel at (x, y) to the given gray level (0x00 = Black, 0xff = White)
    void setPixel(int x, int y, uint8_t grayLevel);

    // Uses SDL to render the current VRAM as a 128×128 image (scaled up)
    void renderFrame();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
};

#endif // GPU_H