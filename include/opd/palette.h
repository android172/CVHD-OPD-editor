#ifndef __PALETTE_H__
#define __PALETTE_H__

#include "color.h"

#include <array>
#include <QHash>
#include <QImage>

struct Palette {
    ushort index = (ushort) -1;
    uchar  size  = 0;

    typedef std::array<Color, 16>::iterator       iterator;
    typedef std::array<Color, 16>::const_iterator const_iterator;

    Color get_color(uchar index) const;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    bool contains(const Color color) const;

    void add_color(const Color color);

    Color&       operator[](size_t index);
    const Color& operator[](size_t index) const;

  private:
    std::array<Color, 16> _colors;
};
typedef Palette* PalettePtr;

struct PalettePair {
    Palette original;
    Palette display;

    void compute_from_image(
        QImage image, QHash<Color, uchar>& out_color_indices
    );
};

#endif // __PALETTE_H__