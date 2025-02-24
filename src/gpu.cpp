#include "gpu.h"

#include "memory.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

// Here are the ChatGPT links to code recommendations
// https://chatgpt.com/share/67bac209-01f0-800e-a6d4-24d4d72a4a07
// https://chatgpt.com/share/67babfe6-d820-800e-a22c-84481ab7fab9


// Scale factor to enlarge the 128x128 display
const int WINDOW_SCALE = 4;
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU() : window(nullptr), renderer(nullptr), texture(nullptr) {
    // Initialize internal VRAM buffer to zeros.
    std::memset(vram, 0, sizeof(vram));

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
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
    if (window == nullptr) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError()
                  << std::endl;
        exit(1);
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        FRAME_WIDTH,
        FRAME_HEIGHT
    );
    if (texture == nullptr) {
        std::cerr << "SDL_CreateTexture Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

GPU::~GPU() {
    if (texture != nullptr)
        SDL_DestroyTexture(texture);
    if (renderer != nullptr)
        SDL_DestroyRenderer(renderer);
    if (window != nullptr)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

uint32_t GPU::getPixelAddress(uint32_t x_coord, uint32_t y_coord) {
    // Calculate pixel index: x + (y * 128)
    return static_cast<uint16_t>(Memory::Address::STACK_END) +
           (x_coord + y_coord * FRAME_WIDTH);
}

void GPU::setPixel(uint32_t x_coord, uint32_t y_coord, uint8_t gray_level) {
    if (x_coord >= FRAME_WIDTH || y_coord >= FRAME_HEIGHT)
        return;
    uint32_t const index = x_coord + (y_coord * FRAME_WIDTH);
    vram[index] = gray_level;
}

void GPU::renderFrame() {
    // Process SDL events (for example, window close events)
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT)
            exit(0);
    }

    // Copy external VRAM (from main memory) into the internal buffer.
    if (externalVRAM != nullptr) {
        std::memcpy(vram, externalVRAM, VRAM_SIZE);
    } else {
        std::cerr << "Warning: External VRAM pointer not set." << std::endl;
    }

    // Convert each 1-byte gray level into a 32-bit ARGB pixel.
    const uint8_t bits_per_byte = 8;
    const uint8_t low_byte = 0xff;
    std::array<uint32_t, VRAM_SIZE> pixels{};
    for (size_t i = 0; i < VRAM_SIZE; ++i) {
        uint8_t const gray = vram[i];
        pixels[i] = (low_byte << bits_per_byte * 3) |
                    (gray << bits_per_byte * 2) | (gray << bits_per_byte) |
                    gray;
    }

    // Update the SDL texture with the pixel data and render it.
    SDL_UpdateTexture(
        texture,
        nullptr,
        pixels.data(),
        FRAME_WIDTH * sizeof(uint32_t)
    );
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void GPU::setExternalVRAM(uint8_t* ptr) { externalVRAM = ptr; }