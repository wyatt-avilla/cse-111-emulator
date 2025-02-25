#pragma once

#include <SDL2/SDL.h>
#include <cstdint>
class Console;

class GPU {
  public:

    GPU(Console* console = nullptr);
    
    static const uint32_t FRAME_WIDTH = 128;
    static const uint32_t FRAME_HEIGHT = 128;
    static const uint32_t VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT; 

    ~GPU();

    static uint32_t getPixelAddress(uint32_t x, uint32_t y);

    void setPixel(uint32_t x, uint32_t y, uint8_t gray_level);

    void renderFrame();

    void setExternalVRAM(uint8_t* ptr);

  private:
    Console* console; 

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    uint8_t vram[VRAM_SIZE];

    uint8_t* externalVRAM = nullptr;
};