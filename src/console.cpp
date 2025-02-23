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
    
    // Check for SDL quit event to exit the emulator
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            std::cerr << "🛑 SDL Quit Event Detected! Stopping execution..." << std::endl;
            stopExecution(); // Stops the main loop
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