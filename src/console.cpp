#include "console.h"

#include <fstream>

Console ::Console(const std::string& slug_file_path) {
    std::ifstream slug_file(slug_file_path);
    if (!slug_file.is_open()) {
        throw std::invalid_argument(
            "Couldn't open: \"" + slug_file_path + "\""
        );
    }

    slug_file.close();
}
