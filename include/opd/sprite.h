#ifndef __SPRITE_H__
#define __SPRITE_H__

#include "gfx_page.h"
#include "frame.h"

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

    void initialize();
    void from_image(
        const QVector<QVector<uchar>>& pixels,
        const short                    x_pos,
        const short                    y_pos,
        const ushort                   width,
        const ushort                   height
    );
    void from_frame_parts(const QVector<FramePartPtr>& frame_parts);

    QImage to_image(
        const Palette& palette,
        bool           with_background  = false,
        float          alpha_multiplier = 1.0f
    ) const;
    void trim();

  private:
    void trim_empty_pixels();
};
typedef std::list<Sprite>::iterator SpritePtr;

#endif // __SPRITE_H__