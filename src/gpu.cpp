#include "gpu.h"
#include <iostream>
#include <cstring>

const int WINDOW_SCALE = 4; // Scale factor to enlarge the image window
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU() : window(nullptr), renderer(nullptr), texture(nullptr) {
    std::memset(vram, 0, sizeof(vram));
    
    // Initialize SDL video subsystem
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    }
    
    // Create an SDL window
    window = SDL_CreateWindow("Banana Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    }
    
    // Create a renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    }
    
    // Create a streaming texture for a 128×128 image in ARGB8888 format
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FRAME_WIDTH, FRAME_HEIGHT);
    if (!texture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
    }
}

GPU::~GPU() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
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
    // Poll for SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            // Exit the program when the window is closed
            exit(0);
        }
    }
    
    // Create a temporary pixel buffer for the texture
    uint32_t pixels[VRAM_SIZE];
    for (int i = 0; i < VRAM_SIZE; ++i) {
        uint8_t gray = vram[i];
        // Convert grayscale value to ARGB (A=255, R=gray, G=gray, B=gray)
        pixels[i] = (255 << 24) | (gray << 16) | (gray << 8) | gray;
    }

    // Update the texture with our pixel buffer
    SDL_UpdateTexture(texture, nullptr, pixels, FRAME_WIDTH * sizeof(uint32_t));
    
    // Clear the renderer
    SDL_RenderClear(renderer);
    // Copy the texture to the renderer (it will be scaled to the window size)
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    // Present the updated renderer to the window
    SDL_RenderPresent(renderer);
}