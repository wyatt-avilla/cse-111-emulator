
#include "controller.h"
#include <iostream>
#include <bitset>
#include <SDL2/SDL.h> 

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
/*
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
    */




    void Controller::updateController() {
        SDL_Event event;
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
    
    // Start with fresh state
    uint8_t new_state = controller_state;
        // Check if keys were pressed or released
        if (keystate[SDL_SCANCODE_Z])      new_state |= CONTROLLER_A_MASK;
        if (keystate[SDL_SCANCODE_X])      new_state |= CONTROLLER_B_MASK;
        if (keystate[SDL_SCANCODE_LSHIFT] || keystate[SDL_SCANCODE_RSHIFT]) new_state |= CONTROLLER_SELECT_MASK;
        if (keystate[SDL_SCANCODE_RETURN]) new_state |= CONTROLLER_START_MASK;
        if (keystate[SDL_SCANCODE_UP])     new_state |= CONTROLLER_UP_MASK;
        if (keystate[SDL_SCANCODE_DOWN])   new_state |= CONTROLLER_DOWN_MASK;
        if (keystate[SDL_SCANCODE_LEFT])   new_state |= CONTROLLER_LEFT_MASK;
        if (keystate[SDL_SCANCODE_RIGHT])  new_state |= CONTROLLER_RIGHT_MASK;
        
        // Update state only if it changed
        if (new_state != controller_state) {
            controller_state = new_state;
            displayControllerState();
        }
      // Update state
      if (new_state != controller_state) {
        controller_state = new_state;
        std::cout << "Controller state: " << std::bitset<8>(controller_state) << std::endl;
    }

if (keystate[SDL_SCANCODE_LSHIFT]) {
    std::cout << "Shift key detected!" << std::endl;
    new_state |= CONTROLLER_SELECT_MASK;
}
    }

// ✅ Print the binary representation of controller state
void Controller::displayControllerState() const {
    std::cout << std::bitset<8>(controller_state) << std::endl;
} 

