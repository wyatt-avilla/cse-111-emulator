#include "filter.h"

void Filter::setColor(uint8_t red, uint8_t green, uint8_t blue) {
    this->color.red = red;
    this->color.green = green;
    this->color.blue = blue;
}

Filter::Color Filter::getColor() { return this->color; }
