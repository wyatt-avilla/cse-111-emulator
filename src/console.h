#include <string>

#pragma once

#include "cpu.h"
#include "gpu.h"
#include "memory.h"
#include "os.h"
#include "controller.h"

#define CONTROLLER_A_MASK      ((uint8_t)0x80)
#define CONTROLLER_B_MASK      ((uint8_t)0x40)
#define CONTROLLER_SELECT_MASK ((uint8_t)0x20)
#define CONTROLLER_START_MASK  ((uint8_t)0x10)
#define CONTROLLER_UP_MASK     ((uint8_t)0x08)
#define CONTROLLER_DOWN_MASK   ((uint8_t)0x04)
#define CONTROLLER_LEFT_MASK   ((uint8_t)0x02)
#define CONTROLLER_RIGHT_MASK  ((uint8_t)0x01)

class Controller;

class Console {
  public:
    Console();
    CPU cpu;
    OS os;
    Memory memory;
    GPU gpu;
    Controller controller;

    void run(const std::string& slug_file_path);
    void stopExecution();
    [[nodiscard]] bool isRunning() const;

  private:
    bool is_running{true};
};
