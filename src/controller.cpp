#include "controller.h"

Controller::Controller() : state(0) {}

void Controller::handleEvent(const SDL_Event& event) {
    uint8_t mask = 0;

    switch (event.key.keysym.sym) {
        case SDLK_a:      mask = CONTROLLER_A_MASK; break;
        case SDLK_b:      mask = CONTROLLER_B_MASK; break;
        case SDLK_RETURN: mask = CONTROLLER_START_MASK; break;
        case SDLK_SPACE:  mask = CONTROLLER_SELECT_MASK; break;
        case SDLK_UP:     mask = CONTROLLER_UP_MASK; break;
        case SDLK_DOWN:   mask = CONTROLLER_DOWN_MASK; break;
        case SDLK_LEFT:   mask = CONTROLLER_LEFT_MASK; break;
        case SDLK_RIGHT:  mask = CONTROLLER_RIGHT_MASK; break;
        default: return;
    }

    if (event.type == SDL_KEYDOWN) {
        state |= mask;
    } else if (event.type == SDL_KEYUP) {
        state &= ~mask;
    }
}

uint8_t Controller::getState() const {
    return state;
}
