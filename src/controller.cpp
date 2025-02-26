#include "controller.h"

#include "console.h"

#include <SDL2/SDL.h>


Controller::Controller(Console* console) : console(console) {}

uint8_t Controller::getState() {
    const auto* const keystate = SDL_GetKeyboardState(nullptr);

    uint8_t new_state = 0;

    if (keystate[SDL_SCANCODE_Z] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::A);
    if (keystate[SDL_SCANCODE_X] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::B);
    if ((keystate[SDL_SCANCODE_LSHIFT] != 0) ||
        (keystate[SDL_SCANCODE_RSHIFT] != 0))
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::SELECT);
    if (keystate[SDL_SCANCODE_RETURN] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::START);
    if (keystate[SDL_SCANCODE_UP] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::UP);
    if (keystate[SDL_SCANCODE_DOWN] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::DOWN);
    if (keystate[SDL_SCANCODE_LEFT] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::LEFT);
    if (keystate[SDL_SCANCODE_RIGHT] != 0)
        new_state |= static_cast<std::underlying_type_t<Mask>>(Mask::RIGHT);

    return new_state;
}