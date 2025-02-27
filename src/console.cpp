#include "console.h"

Console::Console(bool graphics)
    : cpu(CPU(this)), os(OS(this)), memory(Memory(this)), gpu(this),
      controller(Controller(this)), graphics(graphics) {}

void Console::stopExecution() { is_running = false; }

bool Console::graphicalSession() const { return graphics; }

bool Console::isRunning() const { return is_running; }

void Console::run(const std::string& slug_file_path) {
    if (graphicalSession()) {
        gpu.initializeRenderer();
    }

    os.reset(slug_file_path);
    while (isRunning()) {
        os.loopIteration();
    }
}
