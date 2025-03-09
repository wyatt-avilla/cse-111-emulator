#include "vr.h"

#include <SDL2/SDL.h>
#include <array>
#include <cstdint>
#include <stdexcept>

#pragma once

class Console;

class GPU {
  public:
    GPU(Console* console = nullptr);

    static const uint32_t FRAME_WIDTH = 128;
    static const uint32_t FRAME_HEIGHT = 128;
    static const uint32_t VRAM_SIZE = FRAME_WIDTH * FRAME_HEIGHT;

    ~GPU();

    void initializeRenderer();

    static uint32_t getPixelAddress(uint32_t x_coord, uint32_t y_coord);

    void setPixel(uint32_t x_coord, uint32_t y_coord, uint8_t gray_level);

    void renderFrame();

    void setExternalVRAM(uint8_t* ptr);

    void setVideoRecorder(VideoRecorder* recorder);

    void setSelectedColor(uint8_t r, uint8_t g, uint8_t b);
  private:
    

    Console* console;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;

    std::array<uint8_t, VRAM_SIZE> vram{};

    uint8_t* external_vram = nullptr;

    VideoRecorder* video_recorder = nullptr;
    SDL_Color selectedColorMod; 
    
};

class GraphicsException : public std::runtime_error {
  public:
    explicit GraphicsException(const std::string& message)
        : std::runtime_error("Graphics Error: " + message) {}
};
