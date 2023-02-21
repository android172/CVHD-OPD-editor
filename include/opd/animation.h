#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include "frame.h"

struct Animation {
    struct Frame {
        ushort   index = (ushort) -1;
        FramePtr data;
        ushort   delay;
        short    x_offset;
        short    y_offset;
    };

    ushort           index = (ushort) -1;
    QString          name;
    std::list<Frame> frames;
};

typedef std::list<Animation>::iterator        AnimationPtr;
typedef std::list<Animation::Frame>::iterator AnimationFramePtr;

#endif // __ANIMATION_H__