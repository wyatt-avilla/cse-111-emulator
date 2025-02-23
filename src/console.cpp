#include "console.h"

#include "iostream"

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

Console ::Console()
    : cpu(CPU(this)), os(OS(this)), memory(Memory(this)), controllerState(0) {
    SDL_Init(SDL_INIT_VIDEO);
}

Console::~Console() { SDL_Quit(); }

void Console::pollInput() {
    const Uint8* keyboard = SDL_GetKeyboardState(NULL);
    controllerState = 0;

    if (keyboard[SDL_SCANCODE_A])
        controllerState |= CONTROLLER_A_MASK;
    if (keyboard[SDL_SCANCODE_S])
        controllerState |= CONTROLLER_B_MASK;
    if (keyboard[SDL_SCANCODE_TAB])
        controllerState |= CONTROLLER_SELECT_MASK;
    if (keyboard[SDL_SCANCODE_RETURN])
        controllerState |= CONTROLLER_START_MASK;
    if (keyboard[SDL_SCANCODE_UP])
        controllerState |= CONTROLLER_UP_MASK;
    if (keyboard[SDL_SCANCODE_DOWN])
        controllerState |= CONTROLLER_DOWN_MASK;
    if (keyboard[SDL_SCANCODE_LEFT])
        controllerState |= CONTROLLER_LEFT_MASK;
    if (keyboard[SDL_SCANCODE_RIGHT])
        controllerState |= CONTROLLER_RIGHT_MASK;
    // Write the controller byte to memory at address 0x7000
    this->memory.w8u(
        static_cast<uint16_t>(Memory::Address::CONTROLLER_DATA),
        controllerState
    );

    // Optional: For debugging, print the controller byte to the console
    std::cout << "Controller Byte: " << std::bitset<8>(controllerState)
              << std::endl;
}

uint8_t Console::getControllerState() const { return controllerState; }

void Console::stopExecution() {
    is_running = false; // Set the flag to false to stop the execution loop
}

bool Console::isRunning() const { return is_running; }

void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    while (isRunning()) {
        os.loopIteration();
    }
}