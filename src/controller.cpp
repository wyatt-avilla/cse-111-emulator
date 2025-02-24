/*
#include "controller.h"
#include <iostream>
#include <bitset>

#define CONTROLLER_A_MASK      ((uint8_t)0x80)
#define CONTROLLER_B_MASK      ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)
#define CONTROLLER_START_MASK  ((uint8_t)0x10)
#define CONTROLLER_UP_MASK     ((uint8_t)0x08)
#define CONTROLLER_DOWN_MASK   ((uint8_t)0x04)
#define CONTROLLER_LEFT_MASK   ((uint8_t)0x02)
#define CONTROLLER_RIGHT_MASK  ((uint8_t)0x01)

Controller::Controller(Console* console) : console(console), controller_state(0) {}

// ✅ Return the current state of the controller as a bitmask
uint8_t Controller::getState() const {
    return controller_state;
}

// ✅ Update controller state based on user input
void Controller::updateController() {
    uint8_t new_state = 0;

    // Example: Read from standard input (modify this for real input handling)
    char key;
    if (std::cin >> key) {
        switch (key) {
            case 'a': new_state |= CONTROLLER_A_MASK; break;
            case 'b': new_state |= CONTROLLER_B_MASK; break;
            case 's': new_state |= CONTROLLER_SELECT_MASK; break;
            case 't': new_state |= CONTROLLER_START_MASK; break;
            case 'u': new_state |= CONTROLLER_UP_MASK; break;
            case 'd': new_state |= CONTROLLER_DOWN_MASK; break;
            case 'l': new_state |= CONTROLLER_LEFT_MASK; break;
            case 'r': new_state |= CONTROLLER_RIGHT_MASK; break;
            default: break;
        }
    }

    // ✅ Update state only if it changed
    if (new_state != controller_state) {
        controller_state = new_state;
        displayControllerState();
    }
}

// ✅ Print the binary representation of controller state
void Controller::displayControllerState() const {
    std::cout << std::bitset<8>(controller_state) << std::endl;
} */

#include "controller.h"
#include <SDL2/SDL.h>
#include <iostream>
#include <bitset>

#define CONTROLLER_A_MASK      ((uint8_t)0x80)
#define CONTROLLER_B_MASK      ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)
#define CONTROLLER_START_MASK  ((uint8_t)0x10)
#define CONTROLLER_UP_MASK     ((uint8_t)0x08)
#define CONTROLLER_DOWN_MASK   ((uint8_t)0x04)
#define CONTROLLER_LEFT_MASK   ((uint8_t)0x02)
#define CONTROLLER_RIGHT_MASK  ((uint8_t)0x01)

Controller::Controller(Console* console) : console(console), controller_state(0) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

// ✅ Return the current state of the controller as a bitmask
uint8_t Controller::getState() const {
    return controller_state;
}

// ✅ Update controller state based on SDL input
void Controller::updateController() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        bool pressed = (event.type == SDL_KEYDOWN);
        bool released = (event.type == SDL_KEYUP);

        if (pressed || released) {
            uint8_t mask = 0;

            switch (event.key.keysym.sym) {
                case SDLK_a: mask = CONTROLLER_A_MASK; break;       // A Button
                case SDLK_b: mask = CONTROLLER_B_MASK; break;       // B Button
                case SDLK_RETURN: mask = CONTROLLER_START_MASK; break; // Start
                case SDLK_RSHIFT: mask = CONTROLLER_SELECT_MASK; break; // Select
                case SDLK_UP: mask = CONTROLLER_UP_MASK; break;     // D-Pad Up
                case SDLK_DOWN: mask = CONTROLLER_DOWN_MASK; break; // D-Pad Down
                case SDLK_LEFT: mask = CONTROLLER_LEFT_MASK; break; // D-Pad Left
                case SDLK_RIGHT: mask = CONTROLLER_RIGHT_MASK; break; // D-Pad Right
                case SDLK_ESCAPE: console->stopExecution(); return; // Quit Emulator
            }

            if (mask) {
                if (pressed) controller_state |= mask;  // Set bit on key press
                if (released) controller_state &= ~mask; // Clear bit on key release
            }
        }

        if (event.type == SDL_QUIT) {
            console->stopExecution();
        }
    }
}

// ✅ Print the binary representation of controller state
void Controller::displayControllerState() const {
    std::cout << "Controller State: " << std::bitset<8>(controller_state) << std::endl;
}

Controller::~Controller() {
    SDL_Quit();
}