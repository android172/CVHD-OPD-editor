#ifndef __COLOR_H__
#define __COLOR_H__

#include <QHash>

struct Color {
    uchar r;
    uchar g;
    uchar b;

    bool operator==(const Color& other) const;

    QString to_hex();
};
uint qHash(const Color& key, uint seed = 0);

struct ColorPair {
    Color original;
    Color display;

    bool operator==(const ColorPair& other) const;
    bool operator<(const ColorPair& other) const;
};

#endif // __COLOR_H__