#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "gfx_page.h"
#include "palette.h"

#include <QImage>

struct Sprite {
    ushort index = (ushort) -1;

    GFXPagePtr gfx_page;
    ushort     gfx_x_pos;
    ushort     gfx_y_pos;

    short  x_pos;
    short  y_pos;
    ushort width;
    ushort height;

    QVector<QVector<uchar>> pixels;

    bool operator==(const Sprite& other) const;

    void   initialize();
    QImage to_image(const Palette& palette, bool with_background = false) const;
    void   trim();
};
typedef std::list<Sprite>::iterator SpritePtr;

#endif // __SPRITE_H__