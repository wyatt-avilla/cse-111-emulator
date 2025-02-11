#include <string>

#pragma once

class Console;

class OS {
  private:
    Console* c;

  public:
    OS(Console* console);

    void reset(const std::string& filename);
    void loopIteration();
};
