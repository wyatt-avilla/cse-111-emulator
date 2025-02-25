#include "gpu.h"
#include "console.h"
#include "memory.h"

#include <cstdlib>
#include <cstring>
#include <iostream>

// Here are the ChatGPT links to code recommendations
// https://chatgpt.com/share/67bac209-01f0-800e-a6d4-24d4d72a4a07
// https://chatgpt.com/share/67babfe6-d820-800e-a22c-84481ab7fab9


// Define scaling factors for the display window
const int WINDOW_SCALE = 4;
const int WINDOW_WIDTH = GPU::FRAME_WIDTH * WINDOW_SCALE;
const int WINDOW_HEIGHT = GPU::FRAME_HEIGHT * WINDOW_SCALE;

GPU::GPU(Console* console) : console(console), window(nullptr), renderer(nullptr), texture(nullptr) {
    // Initialize VRAM with zeros

    std::memset(vram, 0, sizeof(vram));
 // Initialize SDL for video rendering
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
 // Create an SDL window for displaying graphics
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
    // Create an SDL renderer to handle graphics drawing
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError()
                  << std::endl;
        exit(1);
    }
    // Create an SDL texture for rendering pixel data

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
// Function to get pixel address in VRAM
uint32_t GPU::getPixelAddress(uint32_t x_coord, uint32_t y_coord) {
    // Computes the memory address of a pixel in VRAM
    return static_cast<uint16_t>(Memory::Address::STACK_END) +
           (x_coord + y_coord * FRAME_WIDTH);
}
// Function to set a pixel in VRAM
void GPU::setPixel(uint32_t x_coord, uint32_t y_coord, uint8_t gray_level) {
     // Ensure coordinates are within bounds
    if (x_coord >= FRAME_WIDTH || y_coord >= FRAME_HEIGHT)
        return;
        // Compute index in VRAM and update pixel value
    uint32_t const index = x_coord + (y_coord * FRAME_WIDTH);
    vram[index] = gray_level;
}
// Function to render a frame on the screen

void GPU::renderFrame() {
 // Poll for SDL events (e.g., window close)
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        if (event.type == SDL_QUIT) {
            
            std::cerr << "Quit event received, stopping execution" << std::endl;
            if (console) {
                console->stopExecution();
            } else {
                
                std::cerr << "Console pointer not set, exiting directly" << std::endl;
                exit(0);
            }
        }
    }

// Check if external VRAM is set, otherwise print a warning
    if (externalVRAM != nullptr) {
        std::memcpy(vram, externalVRAM, VRAM_SIZE);
    } else {
        std::cerr << "Warning: External VRAM pointer not set." << std::endl;
    }

    // Prepare a pixel buffer for rendering
    const uint8_t bits_per_byte = 8;
    const uint8_t low_byte = 0xff;
    std::array<uint32_t, VRAM_SIZE> pixels{};
     // Convert VRAM grayscale values to ARGB format
    for (size_t i = 0; i < VRAM_SIZE; ++i) {
        uint8_t const gray = vram[i];
        pixels[i] = (low_byte << bits_per_byte * 3) |
                    (gray << bits_per_byte * 2) | (gray << bits_per_byte) |
                    gray;
    }

        // Update SDL texture with new pixel data
    SDL_UpdateTexture(
        texture,
        nullptr,
        pixels.data(),
        FRAME_WIDTH * sizeof(uint32_t)
    );
    // Clear renderer and draw new frame
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}
// Function to link GPU with external VRAM
void GPU::setExternalVRAM(uint8_t* ptr) { externalVRAM = ptr; }



