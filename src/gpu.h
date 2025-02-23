#ifndef GPU_H
#define GPU_H

#include <cstdint>
#include <SDL2/SDL.h>

class GPU {
public:
    static const int FRAME_WIDTH = 128;
    static const int FRAME_HEIGHT = 128;
    static const int VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT;

    GPU();
    ~GPU();

    // Returns the index in VRAM for pixel (x, y)
    int getPixelAddress(int x, int y);

    // Sets the pixel at (x, y) in the internal buffer (0x00 = Black, 0xff = White)
    void setPixel(int x, int y, uint8_t grayLevel);

    // Renders the internal VRAM buffer to the screen via SDL
    void renderFrame();

    // Setter to pass the external VRAM pointer (points to main memory offset 0x3000)
    void setExternalVRAM(uint8_t* ptr);

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    // GPU's own internal VRAM buffer
    uint8_t vram[VRAM_SIZE];

    // Pointer to the external memory's VRAM region
    uint8_t* externalVRAM = nullptr;
};

#endif // GPU_H