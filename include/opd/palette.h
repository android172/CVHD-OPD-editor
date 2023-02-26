#ifndef __PALETTE_H__
#define __PALETTE_H__

#include "color.h"
#include <array>

struct Palette {
    ushort index = (ushort) -1;
    uchar  size  = 0;

    typedef std::array<Color, 16>::iterator       iterator;
    typedef std::array<Color, 16>::const_iterator const_iterator;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    Color&       operator[](size_t index);
    const Color& operator[](size_t index) const;

  private:
    std::array<Color, 16> _colors;
};
typedef Palette* PalettePtr;

#endif // __PALETTE_H__