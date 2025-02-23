#include "console.h"

#include <SDL2/SDL.h>
#include <bitset>
#include <iostream>

#define CONTROLLER_A_MASK ((uint8_t) 0x80)
#define CONTROLLER_B_MASK ((uint8_t) 0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t) 0x20)
#define CONTROLLER_START_MASK ((uint8_t) 0x10)
#define CONTROLLER_UP_MASK ((uint8_t) 0x08)
#define CONTROLLER_DOWN_MASK ((uint8_t) 0x04)
#define CONTROLLER_LEFT_MASK ((uint8_t) 0x02)
#define CONTROLLER_RIGHT_MASK ((uint8_t) 0x01)

Console::Console()
    : cpu(CPU(this)), os(OS(this)), memory(Memory(this)), gpu(GPU()) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "🚨 SDL Initialization Failed: " << SDL_GetError() << std::endl;
        exit(EXIT_FAILURE);
    }
}

Console::~Console() {
    SDL_Quit();
}

void Console::pollInput() {
    SDL_Event event;
    
    // Process SDL events (ensuring quit events are handled)
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cerr << "🛑 SDL Quit Event Detected! Exiting..." << std::endl;
            stopExecution();  // Stop execution properly
            return;
        }
    }

    // Get keyboard state
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    controllerState = 0;

    if (keyboard[SDL_SCANCODE_A]) controllerState |= CONTROLLER_A_MASK;
    if (keyboard[SDL_SCANCODE_S]) controllerState |= CONTROLLER_B_MASK;
    if (keyboard[SDL_SCANCODE_TAB]) controllerState |= CONTROLLER_SELECT_MASK;
    if (keyboard[SDL_SCANCODE_RETURN]) controllerState |= CONTROLLER_START_MASK;
    if (keyboard[SDL_SCANCODE_UP]) controllerState |= CONTROLLER_UP_MASK;
    if (keyboard[SDL_SCANCODE_DOWN]) controllerState |= CONTROLLER_DOWN_MASK;
    if (keyboard[SDL_SCANCODE_LEFT]) controllerState |= CONTROLLER_LEFT_MASK;
    if (keyboard[SDL_SCANCODE_RIGHT]) controllerState |= CONTROLLER_RIGHT_MASK;

    std::cerr << "🔍 Controller Byte: " << std::bitset<8>(controllerState) << std::endl;

    // 🛑 Only write to memory if controllerState is NOT zero
    if (controllerState != 0) {
        std::cerr << "✅ Writing to CONTROLLER_DATA (0x7000): " << std::bitset<8>(controllerState) << std::endl;
        this->memory.w8u(
            static_cast<uint16_t>(Memory::Address::CONTROLLER_DATA),
            controllerState
        );
    } else {
        std::cerr << "⚠️ Skipping write to 0x7000 because controllerState is 0." << std::endl;
    }
}

uint8_t Console::getControllerState() const {
    return controllerState;
}

void Console::stopExecution() {
    std::cerr << "🛑 Emulator Stopping!" << std::endl;
    is_running = false; 
}

bool Console::isRunning() const {
    return is_running;
}

void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    while (isRunning()) {
        os.loopIteration();
    }
}