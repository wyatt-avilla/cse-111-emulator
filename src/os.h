#pragma once


#include <string>

class Console;

class OS {
  private:
    Console* c;

  public:
    OS(Console* c_);

    void reset(const std::string& filename);
    void loop_iteration();
};
