#include "gpu.h"
#include <iostream>
#include <cstdlib>
#include <cstring>

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
    
    window = SDL_CreateWindow("Banana Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
    
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                FRAME_WIDTH, FRAME_HEIGHT);
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

int GPU::getPixelAddress(int x, int y) const {
    // Calculate pixel index: x + (y * 128)
    return 0x3000 + (x + y * FRAME_WIDTH);
}

void GPU::setPixel(int x, int y, uint8_t grayLevel) {
    if (x < 0 || x >= FRAME_WIDTH || y < 0 || y >= FRAME_HEIGHT)
        return;
    int index = x + (y * FRAME_WIDTH);
    vram[index] = grayLevel;
}

void GPU::renderFrame() {
    // Process SDL events (for example, window close events)
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT)
            exit(0);
    }
    
    // Copy external VRAM (from main memory) into the internal buffer.
    if (externalVRAM) {
        std::memcpy(vram, externalVRAM, VRAM_SIZE);
    } else {
        std::cerr << "Warning: External VRAM pointer not set." << std::endl;
    }
    
    // Debug: Dump the first 16 bytes of the internal VRAM buffer.
    std::cerr << "GPU::renderFrame: Dumping first 16 bytes of VRAM:" << std::endl;
    for (int i = 0; i < 16; ++i) {
        std::cerr << "  vram[" << i << "] = 0x" << std::hex << int(vram[i]) << std::endl;
    }
    
    // Convert each 1-byte gray level into a 32-bit ARGB pixel.
    uint32_t pixels[VRAM_SIZE];
    for (int i = 0; i < VRAM_SIZE; ++i) {
        uint8_t gray = vram[i];
        pixels[i] = (0xFF << 24) | (gray << 16) | (gray << 8) | gray;
    }
    
    // Update the SDL texture with the pixel data and render it.
    SDL_UpdateTexture(texture, nullptr, pixels, FRAME_WIDTH * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);
}

void GPU::setExternalVRAM(uint8_t* ptr) {
    externalVRAM = ptr;
}