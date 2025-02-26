#include "console.h"

Console::Console()
    : cpu(CPU(this)), os(OS(this)), memory(Memory(this)), gpu(this),
      controller(Controller(this)) {}

void Console::stopExecution() { is_running = false; }

bool Console::isRunning() const { return is_running; }

void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    while (isRunning()) {
        os.loopIteration();
    }
}
