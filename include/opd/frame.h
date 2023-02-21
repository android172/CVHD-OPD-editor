#ifndef __FRAME_H__
#define __FRAME_H__

#include "sprite.h"
#include "palette.h"

struct Frame {
    struct Part {
        ushort     index = (ushort) -1;
        SpritePtr  sprite;
        PalettePtr palette;
        short      x_offset;
        short      y_offset;
        uchar      flip_mode;
    };

    struct HitBox {
        ushort index = (ushort) -1;
        short  x_position;
        short  y_position;
        ushort width;
        ushort height;
    };

    ushort  index = (ushort) -1;
    QString name;
    short   x_offset;
    short   y_offset;

    std::list<Part>   parts;
    std::list<HitBox> hitboxes;

    uint uses = 0;

    void initialize();
};
typedef std::list<Frame>::iterator         FramePtr;
typedef std::list<Frame::Part>::iterator   FramePartPtr;
typedef std::list<Frame::HitBox>::iterator HitBoxPtr;

#endif // __FRAME_H__