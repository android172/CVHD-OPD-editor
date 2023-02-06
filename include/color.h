#ifndef COLOR_H
#define COLOR_H

#include <QString>
#include <sstream>

struct Color {
    uchar r;
    uchar g;
    uchar b;

    bool operator==(const Color& other) const {
        return r == other.r && g == other.g && b == other.b;
    }

    QString to_hex() {
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
};

inline uint qHash(const Color& key, uint seed = 0) {
    return ((key.r << 16) | (key.g << 8) | key.b) ^ seed;
}

struct ColorPair {
    Color original;
    Color display;

    bool operator==(const ColorPair& other) const {
        return original.r == other.original.r && //
               original.g == other.original.g && //
               original.b == other.original.b;
    }

    bool operator<(const ColorPair& other) const {
        int d = other.original.r - original.r;
        if (d) return d < 0;
        d = other.original.g - original.g;
        if (d) return d < 0;
        d = other.original.b - original.b;
        return d < 0;
    }
};

#endif // COLOR_H
