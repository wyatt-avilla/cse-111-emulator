#pragma once

#include <cstdint>
#include "console.h"

#define CONTROLLER_A_MASK      ((uint8_t)0x80)
#define CONTROLLER_B_MASK      ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)
#define CONTROLLER_START_MASK  ((uint8_t)0x10)
#define CONTROLLER_UP_MASK     ((uint8_t)0x08)
#define CONTROLLER_DOWN_MASK   ((uint8_t)0x04)
#define CONTROLLER_LEFT_MASK   ((uint8_t)0x02)
#define CONTROLLER_RIGHT_MASK  ((uint8_t)0x01)


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

