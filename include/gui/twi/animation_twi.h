#ifndef __ANIMATION_TWI_H__
#define __ANIMATION_TWI_H__

#include <QTreeWidgetItem>

#include "frame_twi.h"
#include "util.h"

class AnimationTwi : public QTreeWidgetItem {
  public:
    AnimationPtr animation;

    AnimationTwi(const AnimationPtr animation)
        : QTreeWidgetItem(), animation(animation) {
        compute_name();
        // Add frames for this animation
        ForEach(frame, animation->frames) add_frame(frame);
    }
    ~AnimationTwi() {}

    void compute_name() {
        const auto animation_name =
            (animation->name.size())
                ? animation->name
                : "animation " + QString::number(animation->index);
        setText(0, animation_name);
    }

    FrameTwi* add_frame(AnimationFramePtr frame) {
        const auto frame_twi = new FrameTwi(frame->data, frame);
        addChild(frame_twi);
        return frame_twi;
    }

    FrameTwi* get_frame(const int i) {
        if (i >= animation->frames.size()) return nullptr;
        return dynamic_cast<FrameTwi*>(child(i));
    }
};

#endif // __ANIMATION_TWI_H__