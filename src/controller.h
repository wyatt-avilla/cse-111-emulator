/*
#pragma once

#include <cstdint>
#include "console.h"

class Console; // ✅ Forward declaration to avoid circular dependencies

class Controller {
  public:
    explicit Controller(Console* console); // ✅ Constructor
    uint8_t getState() const;              // ✅ Get the current controller state
    void updateController();               // ✅ Update state based on user input
    void displayControllerState() const;   // ✅ Print binary controller state

  private:
    Console* console;
    uint8_t controller_state = 0;  // ✅ Stores button states as a bitmask
};

*/
#pragma once

#include <cstdint>
#include <SDL2/SDL.h>  // ✅ Include SDL for input handling

class Console;

class Controller {
  public:
    explicit Controller(Console* console);
    uint8_t getState() const;
    void updateController();  // ✅ Update function to handle SDL input
    void displayControllerState() const;

  private:
    Console* console;
    uint8_t controller_state;
};
