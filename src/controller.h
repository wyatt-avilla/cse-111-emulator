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


class Console; 

class Controller {
  public:
    explicit Controller(Console* console); 
    ~Controller();
    uint8_t getState() const;              
    void updateController();               
    void displayControllerState() const;   

  private:
    Console* console;
    uint8_t controller_state = 0;  
};

