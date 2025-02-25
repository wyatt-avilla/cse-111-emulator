#include "console.h"

Console::Console()
    : cpu(CPU(this)), os(OS(this)), memory(Memory(this)), gpu(this),
      controller(Controller(this)) {}

void Console::stopExecution() {
    is_running = false; // Set the flag to false to stop the execution loop
}

bool Console::isRunning() const { return is_running; }

void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    while (isRunning()) {
        controller.updateController();
        os.loopIteration();
    }
}
uint8_t Console::getControllerState() const {
    // This will be implemented after we add the controller
    return 0; // Temporary placeholder
}