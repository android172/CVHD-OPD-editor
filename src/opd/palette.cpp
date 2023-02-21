#include "opd/palette.h"

Palette::iterator Palette::begin() { return _colors.begin(); }
Palette::iterator Palette::end() { return _colors.end(); }

Palette::const_iterator Palette::begin() const { return _colors.begin(); }
Palette::const_iterator Palette::end() const { return _colors.end(); }

Color& Palette::operator[](size_t index) {
    if (index >= 16) throw std::runtime_error("Index out of range (0 - 15).");
    return _colors[index];
}
const Color& Palette::operator[](size_t index) const {
    if (index >= 16) throw std::runtime_error("Index out of range (0 - 15).");
    return _colors[index];
}