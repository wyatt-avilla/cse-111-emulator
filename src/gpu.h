#pragma once

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>

class GPU {
  public:
    // ✅ Define resolution constants
    static constexpr uint32_t FRAME_WIDTH = 128;
    static constexpr uint32_t FRAME_HEIGHT = 128;
    static constexpr uint32_t VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT; // 16384 pixels

    GPU();
    ~GPU();

    void renderFrame();  // ✅ Ensure function is properly declared
    void setExternalVRAM(uint8_t* ptr);

  private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    // ✅ Ensure `std::array` is properly defined and initialized
    std::array<uint32_t, VRAM_SIZE> pixels = {}; 

    // ✅ Internal VRAM buffer (1-byte grayscale per pixel)
    std::array<uint8_t, VRAM_SIZE> vram = {};  

    // ✅ External VRAM reference (should point to main memory)
    uint8_t* externalVRAM = nullptr;
};
