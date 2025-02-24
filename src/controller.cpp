#include "controller.h"

Controller::Controller(Console* console) : console(console) {}

void Controller::update() {
    // Read controller state from memory-mapped IO (0x7000)
    uint8_t new_state = console->memory.l8u(IO_CONTROLLER_DATA);

    // If the state has changed, update and print
    if (new_state != controller_state) {
        controller_state = new_state;
        displayState();
    }
}

void Controller::displayState() const {
    std::cout << "Controller State: ";

    // Print binary representation of button presses
    for (int i = 7; i >= 0; --i) {
        std::cout << ((controller_state >> i) & 1);
    }
    std::cout << " | Buttons Pressed: ";

    // Print button names based on bit masks
    if (controller_state & CONTROLLER_A_MASK)      std::cout << "A ";
    if (controller_state & CONTROLLER_B_MASK)      std::cout << "B ";
    if (controller_state & CONTROLLER_SELECT_MASK) std::cout << "SELECT ";
    if (controller_state & CONTROLLER_START_MASK)  std::cout << "START ";
    if (controller_state & CONTROLLER_UP_MASK)     std::cout << "UP ";
    if (controller_state & CONTROLLER_DOWN_MASK)   std::cout << "DOWN ";
    if (controller_state & CONTROLLER_LEFT_MASK)   std::cout << "LEFT ";
    if (controller_state & CONTROLLER_RIGHT_MASK)  std::cout << "RIGHT ";

    std::cout << std::endl;
}

// Read from stdin (memory-mapped at 0x7100)
uint8_t Controller::readStdin() {
    return console->memory.l8u(IO_DEBUG_STDIN);
}

// Write to stdout (memory-mapped at 0x7110)
void Controller::writeStdout(uint8_t value) {
    console->memory.w8u(IO_DEBUG_STDOUT, value);
    std::cout << static_cast<char>(value);
}

// Write to stderr (memory-mapped at 0x7120)
void Controller::writeStderr(uint8_t value) {
    console->memory.w8u(IO_DEBUG_STDERR, value);
    std::cerr << static_cast<char>(value);
}

// Stop execution (memory-mapped at 0x7200)
void Controller::stopExecution() {
    console->memory.w8u(IO_STOP_EXECUTION, 0);
    console->stopExecution();
}