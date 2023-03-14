#include "opd/color.h"

#include <sstream>

bool Color::operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b;
}
bool Color::operator<(const Color& other) const {
    int d = other.r - r;
    if (d) return d < 0;
    d = other.g - g;
    if (d) return d < 0;
    d = other.b - b;
    return d < 0;
}

QString Color::to_hex() {
    std::ostringstream red, green, blue;

    red << std::hex << (int) r;
    green << std::hex << (int) g;
    blue << std::hex << (int) b;

    // Add blue
    auto result = blue.str();
    if (result.size() < 2) result = "0" + result;
    // Add green
    result = green.str() + result;
    if (result.size() < 4) result = "0" + result;
    // Add red
    result = red.str() + result;
    if (result.size() < 6) result = "0" + result;

    return QString::fromUtf8(result.c_str());
}

uint qHash(const Color& key, uint seed) {
    return ((key.r << 16) | (key.g << 8) | key.b) ^ seed;
}