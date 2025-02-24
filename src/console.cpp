#include "console.h"
#include "controller.h"
Console ::Console() : cpu(this), os(this), memory(this), gpu() {
    controller = new Controller(this);  // ✅ Allocate Controller dynamically
}
void Console::stopExecution() {
    is_running = false; // Set the flag to false to stop the execution loop
}

Console::~Console() {
    delete controller;  // ✅ Free memory
}

bool Console::isRunning() const { return is_running; }

void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    while (isRunning()) {
        controller->updateController();
        os.loopIteration();
    }
}