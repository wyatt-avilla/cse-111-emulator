#include "gpu.h"

#include "bit_definitions.h"
#include "console.h"
#include "memory.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

const int WINDOW_SCALE = 4;
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU(Console* console)
    : console(console), window(nullptr), renderer(nullptr), texture(nullptr) {}

GPU::~GPU() {
    if (texture != nullptr)
        SDL_DestroyTexture(texture);
    if (renderer != nullptr)
        SDL_DestroyRenderer(renderer);
    if (window != nullptr)
        SDL_DestroyWindow(window);
    SDL_Quit();
}

void GPU::initializeRenderer() {
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

uint32_t GPU::getPixelAddress(const uint32_t x_coord, const uint32_t y_coord) {
    return static_cast<std::underlying_type_t<Memory::Address>>(
               Memory::Address::STACK_END
           ) +
           (x_coord + y_coord * FRAME_WIDTH);
}

void GPU::setPixel(
    const uint32_t x_coord,
    const uint32_t y_coord,
    const uint8_t gray_level
) {
    if (x_coord >= FRAME_WIDTH || y_coord >= FRAME_HEIGHT)
        return;
    uint32_t const index = x_coord + (y_coord * FRAME_WIDTH);
    vram[index] = gray_level;
}

void GPU::renderFrame() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            std::cerr << "Quit event received, stopping execution" << std::endl;
            console->stopExecution();
        }
    }

    std::memcpy(vram.begin(), external_vram, VRAM_SIZE);

    std::array<uint32_t, VRAM_SIZE> pixels{};
    for (size_t i = 0; i < VRAM_SIZE; ++i) {
        uint8_t const gray = vram[i];
        pixels[i] = (BYTE_MASK << BITS_PER_BYTE * 3) |
                    (gray << BITS_PER_BYTE * 2) | (gray << BITS_PER_BYTE) |
                    gray;
    }

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

void GPU::setExternalVRAM(uint8_t* ptr) { external_vram = ptr; }
