#include "controller.h"
#include <iostream>
#include <bitset>
#include <SDL2/SDL.h>  // Use SDL2 for keyboard input

#define CONTROLLER_A_MASK      ((uint8_t)0x80)
#define CONTROLLER_B_MASK      ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)  // SELECT = Left Shift
#define CONTROLLER_START_MASK  ((uint8_t)0x10)  // START = Enter
#define CONTROLLER_UP_MASK     ((uint8_t)0x08)  // UP = Arrow Up
#define CONTROLLER_DOWN_MASK   ((uint8_t)0x04)  // DOWN = Arrow Down
#define CONTROLLER_LEFT_MASK   ((uint8_t)0x02)  // LEFT = Arrow Left
#define CONTROLLER_RIGHT_MASK  ((uint8_t)0x01)  // RIGHT = Arrow Right

Controller::Controller(Console* console) : console(console), controller_state(0) {}

// ✅ Return the current state of the controller
uint8_t Controller::getState() const {
    return controller_state;
}

// ✅ Update controller state based on SDL2 input
void Controller::updateController() {
    SDL_Event event;
    uint8_t new_state = 0;

    while (SDL_PollEvent(&event)) {  // Process all key events
        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            bool key_pressed = (event.type == SDL_KEYDOWN);  // Check if key was pressed

            switch (event.key.keysym.sym) {
                case SDLK_a:  // A button
                    if (key_pressed) new_state |= CONTROLLER_A_MASK;
                    break;
                case SDLK_b:  // B button
                    if (key_pressed) new_state |= CONTROLLER_B_MASK;
                    break;
                case SDLK_LSHIFT:  // SELECT (Left Shift)
                    if (key_pressed) new_state |= CONTROLLER_SELECT_MASK;
                    break;
                case SDLK_RETURN:  // START (Enter)
                    if (key_pressed) new_state |= CONTROLLER_START_MASK;
                    break;
                case SDLK_UP:  // UP Arrow
                    if (key_pressed) new_state |= CONTROLLER_UP_MASK;
                    break;
                case SDLK_DOWN:  // DOWN Arrow
                    if (key_pressed) new_state |= CONTROLLER_DOWN_MASK;
                    break;
                case SDLK_LEFT:  // LEFT Arrow
                    if (key_pressed) new_state |= CONTROLLER_LEFT_MASK;
                    break;
                case SDLK_RIGHT:  // RIGHT Arrow
                    if (key_pressed) new_state |= CONTROLLER_RIGHT_MASK;
                    break;
                default:
                    break;
            }
        }
    }

    // ✅ Update only if the state changed
    if (new_state != controller_state) {
        controller_state = new_state;
        displayControllerState();
    }
}

// ✅ Print the binary representation of controller state
void Controller::displayControllerState() const {
    std::cout << "Controller State: " << std::bitset<8>(controller_state) 
              << " (0x" << std::hex << (int)controller_state << ")" << std::endl;
}