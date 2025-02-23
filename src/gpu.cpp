#include "gpu.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

// Define window scaling parameters (unchanged)
const int WINDOW_SCALE = 4;
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU() : window(nullptr), renderer(nullptr), texture(nullptr) {
    // vramPtr will be set later via setVRAMPointer()

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    window = SDL_CreateWindow("Banana Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, FRAME_WIDTH, FRAME_HEIGHT);
    if (!texture) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        exit(1);
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
    if (vramPtr) {
        vramPtr[index] = grayLevel;
    }
}

void GPU::renderFrame() {
    // Process SDL events to allow window closing via the X button
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
    }
    
    // Ensure vramPtr is set before rendering
    if (!vramPtr) {
        std::cerr << "Error: VRAM pointer not set!" << std::endl;
        return;
    }

    // Convert the 128x128 grayscale bytes in vramPtr to 32-bit ARGB pixels
    uint32_t pixels[VRAM_SIZE];
    for (int i = 0; i < VRAM_SIZE; ++i) {
        uint8_t gray = vramPtr[i];
        pixels[i] = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;
    }
    
    // Update the texture with our pixel buffer
    SDL_UpdateTexture(texture, nullptr, pixels, FRAME_WIDTH * sizeof(uint32_t));
    
    // Clear the renderer, copy the texture, and present the updated image
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void GPU::setVRAMPointer(uint8_t* ptr) {
    vramPtr = ptr;
}
