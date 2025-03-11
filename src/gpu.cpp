#include "gpu.h"

#include "bit_definitions.h"
#include "console.h"
#include "memory.h"
#include "vr.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

const int32_t WINDOW_SCALE = 4;
const int32_t WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int32_t WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU(Console* console)
    : console(console), window(nullptr), renderer(nullptr), texture(nullptr) {}

GPU::~GPU() {
    if (texture != nullptr) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
    if (renderer != nullptr) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window != nullptr) {
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
    }
}

void GPU::initializeRenderer() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw GraphicsException(
            std::string("SDL_Init Error: ") + SDL_GetError()
        );
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
        throw GraphicsException(
            std::string("SDL_CreateWindow Error: ") + SDL_GetError()
        );
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        throw GraphicsException(
            std::string("SDL_CreateRenderer Error: ") + SDL_GetError()
        );
    }

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        FRAME_WIDTH,
        FRAME_HEIGHT
    );

    if (texture == nullptr) {
        throw GraphicsException(
            std::string("SDL_CreateTexture Error: ") + SDL_GetError()
        );
    }
}

uint32_t GPU::getPixelAddress(const uint32_t x_coord, const uint32_t y_coord) {
    return static_cast<std::underlying_type_t<Memory::Address>>(
               Memory::Address::STACK_END
           ) +
           (x_coord + y_coord * FRAME_WIDTH);
}

void GPU::renderFrame() { // NOLINT(readability-function-size)
    SDL_Event event;

    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            std::cerr << "Quit event received, stopping execution" << std::endl;
            SDL_DestroyWindow(window);
            console->stopExecution();
        }
    }

    std::array<uint32_t, VRAM_SIZE> pixels{};
    const uint8_t* const vram =
        console->memory.getPointerToMemArray() +
        static_cast<std::underlying_type_t<Memory::Address>>(
            Memory::Address::VRAM_START
        );

    // Update pixel data based on VRAM content
    for (size_t i = 0; i < VRAM_SIZE; ++i) {
        uint8_t const gray = vram[i];
        pixels[i] = (BYTE_MASK << BITS_PER_BYTE * 3) |
                    (gray << BITS_PER_BYTE * 2) | (gray << BITS_PER_BYTE) |
                    gray;
    }

    // Apply color tint AFTER updating texture content
    const Filter::Color color = console->filter.getColor();
    int32_t const result =
        SDL_SetTextureColorMod(texture, color.red, color.green, color.blue);
    if (result != 0) {
        std::cerr << "Failed to apply color in renderFrame: " << SDL_GetError()
                  << std::endl;
    }
    console->video_recorder.addFrame(vram);

    // Update the texture with new pixel data
    SDL_UpdateTexture(
        texture,
        nullptr,
        pixels.data(),
        FRAME_WIDTH * sizeof(uint32_t)
    );

    // Render the texture to the screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
