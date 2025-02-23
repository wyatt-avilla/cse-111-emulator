#include "gpu.h"
#include <iostream>
#include <cstring>

const int WINDOW_SCALE = 4;
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU() : window(nullptr), renderer(nullptr), texture(nullptr) {
    // ✅ Initialize VRAM with zeros
    vram.fill(0);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "🚨 SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    window = SDL_CreateWindow(
        "Banana Emulator",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "🚨 SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "🚨 SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        FRAME_WIDTH,
        FRAME_HEIGHT
    );

    if (!texture) {
        std::cerr << "🚨 SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

GPU::~GPU() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

void GPU::renderFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            exit(0);
        }
    }

    if (externalVRAM == nullptr) {
        std::cerr << "🚨 Warning: externalVRAM not set. Skipping render." << std::endl;
        return;
    }

    // ✅ Copy external VRAM into internal buffer
    std::memcpy(vram.data(), externalVRAM, VRAM_SIZE);

    // ✅ Convert grayscale bytes to 32-bit ARGB pixels
    for (size_t i = 0; i < VRAM_SIZE; ++i) {
        uint8_t gray = vram[i];
        pixels[i] = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;
    }

    // ✅ Fix: Use `.data()` instead of `[]`
    SDL_UpdateTexture(texture, nullptr, pixels.data(), FRAME_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void GPU::setExternalVRAM(uint8_t* ptr) {
    externalVRAM = ptr;
}