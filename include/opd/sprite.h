#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "gfx_page.h"

struct Sprite {
    ushort     index = (ushort) -1;
    GFXPagePtr gfx_page;
    ushort     gfx_x_pos;
    ushort     gfx_y_pos;
    ushort     width;
    ushort     height;

    QVector<QVector<uchar>> pixels;

    bool operator==(const Sprite& other) const;

    void initialize();

  private:
    ushort _previous_width;
    ushort _previous_height;
};
typedef std::list<Sprite>::iterator SpritePtr;

#endif // __SPRITE_H__