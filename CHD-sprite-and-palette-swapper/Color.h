#ifndef COLOR_H
#define COLOR_H

#include <sstream>

struct Color {
    unsigned char r;
    unsigned char g;
    unsigned char b;

    bool operator==(const Color& other) const {
        return r == other.r &&
               g == other.g &&
               b == other.b;
    }

    QString to_hex() {
        std::ostringstream red, green, blue;
        red << std::hex << (int)r;
        green << std::hex << (int)g;
        blue << std::hex << (int)b;
        auto result = blue.str();
        if (result.size() < 2) result = "0" + result;
        result = green.str() + result;
        if (result.size() < 4) result = "0" + result;
        result = red.str() + result;
        if (result.size() < 6) result = "0" + result;
        return QString::fromUtf8(result.c_str());
    }
};

namespace std {
template<>
struct hash<Color> {
    size_t operator()(const Color& color) {
        return (color.r << 16) | (color.g << 8) | color.b;
    }
};
} // std

#endif // COLOR_H
