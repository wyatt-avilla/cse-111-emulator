#include "console.h"

Console ::Console() : cpu(CPU(this)), os(OS(this)) {}
void Console::stopExecution() {
    is_running = false; // Set the flag to false to stop the execution loop
}
void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    while (is_running) {
        os.loopIteration();
    }
}
