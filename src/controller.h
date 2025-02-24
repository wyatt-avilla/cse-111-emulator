#pragma once

#include <cstdint>
#include "console.h"

class Console; // ✅ Forward declaration to avoid circular dependencies

class Controller {
  public:
    explicit Controller(Console* console); // ✅ Constructor
    ~Controller();
    uint8_t getState() const;              // ✅ Get the current controller state
    void updateController();               // ✅ Update state based on user input
    void displayControllerState() const;   // ✅ Print binary controller state

  private:
    Console* console;
    uint8_t controller_state = 0;  // ✅ Stores button states as a bitmask
};

