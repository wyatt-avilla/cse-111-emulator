#include "console.h"

Console ::Console()
    : cpu(CPU(this)), os(OS(this)), memory(BananaMemory(this)) {}

void Console::run(const std::string& slug_file_path) {
    os.reset(slug_file_path);
    os.loop();
}
