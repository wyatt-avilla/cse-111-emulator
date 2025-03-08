#include "gpu.h"
#include "filter.h"

#include "bit_definitions.h"
#include "console.h"
#include "memory.h"
#include "vr.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

const int WINDOW_SCALE = 4;
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

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

    if (video_recorder != nullptr) {
        video_recorder->addFrame(vram.data());
    }

    std::array<uint32_t, 65536> pixels{};  

    std::cout << "Before applying filter:" << std::endl;
    for (size_t i = 0; i < 10; ++i) { // Print first 10 values for debugging
        std::cout << pixels[i] << std::endl;
    }

    // Apply the background color filter with the desired RGB values
    Filter::applyBackgroundColorFilter(pixels, 1, 6, 5); // Set background to black (you can change RGB values as needed)

    std::cout << "After applying filter:" << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        std::cout << pixels[i] << std::endl;
    }
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

void GPU::setVideoRecorder(VideoRecorder* recorder) {
    video_recorder = recorder;
}
