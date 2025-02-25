#include "controller.h"

#include "console.h"

#include <SDL2/SDL.h>


Controller::Controller(Console* console) : console(console) {}

uint8_t Controller::getState() {
    const auto* const keystate = SDL_GetKeyboardState(nullptr);

    uint8_t new_state = 0;

    if (keystate[SDL_SCANCODE_Z] != 0)
        new_state |= static_cast<uint8_t>(Mask::A);
    if (keystate[SDL_SCANCODE_X] != 0)
        new_state |= static_cast<uint8_t>(Mask::B);
    if ((keystate[SDL_SCANCODE_LSHIFT] != 0) ||
        (keystate[SDL_SCANCODE_RSHIFT] != 0))
        new_state |= static_cast<uint8_t>(Mask::SELECT);
    if (keystate[SDL_SCANCODE_RETURN] != 0)
        new_state |= static_cast<uint8_t>(Mask::START);
    if (keystate[SDL_SCANCODE_UP] != 0)
        new_state |= static_cast<uint8_t>(Mask::UP);
    if (keystate[SDL_SCANCODE_DOWN] != 0)
        new_state |= static_cast<uint8_t>(Mask::DOWN);
    if (keystate[SDL_SCANCODE_LEFT] != 0)
        new_state |= static_cast<uint8_t>(Mask::LEFT);
    if (keystate[SDL_SCANCODE_RIGHT] != 0)
        new_state |= static_cast<uint8_t>(Mask::RIGHT);

    return new_state;
}
