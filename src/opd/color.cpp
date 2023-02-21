#include "opd/color.h"

#include <sstream>

// -----------------------------------------------------------------------------
// Color
// -----------------------------------------------------------------------------

bool Color::operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b;
}

QString Color::to_hex() {
    std::ostringstream red, green, blue;
    red << std::hex << (int) r;
    green << std::hex << (int) g;
    blue << std::hex << (int) b;
    auto result = blue.str();
    if (result.size() < 2) result = "0" + result;
    result = green.str() + result;
    if (result.size() < 4) result = "0" + result;
    result = red.str() + result;
    if (result.size() < 6) result = "0" + result;
    return QString::fromUtf8(result.c_str());
}

uint qHash(const Color& key, uint seed) {
    return ((key.r << 16) | (key.g << 8) | key.b) ^ seed;
}

// -----------------------------------------------------------------------------
// Color pair
// -----------------------------------------------------------------------------

bool ColorPair::operator==(const ColorPair& other) const {
    return original.r == other.original.r && //
           original.g == other.original.g && //
           original.b == other.original.b;
}
bool ColorPair::operator<(const ColorPair& other) const {
    int d = other.original.r - original.r;
    if (d) return d < 0;
    d = other.original.g - original.g;
    if (d) return d < 0;
    d = other.original.b - original.b;
    return d < 0;
}
