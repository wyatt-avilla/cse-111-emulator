#include <cstdint>
#include <string>

#pragma once

class Console;

class OS {
  private:
    Console* c;
    const uint16_t PC_RESET_VAL = 0xfffc;

    void fetchDecodeExecuteLoop();

  public:
    OS(Console* console);

    void reset(const std::string& filename);
    void loopIteration();
    void setup();
};