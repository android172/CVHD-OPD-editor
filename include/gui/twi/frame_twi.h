#ifndef __FRAME_TWI_H__
#define __FRAME_TWI_H__

#include <QTreeWidgetItem>

#include "opd/animation.h"

class FrameTwi : public QTreeWidgetItem {
  public:
    const FramePtr    frame_info;
    AnimationFramePtr animation_info;

    FrameTwi(FramePtr frame_info, AnimationFramePtr animation_info)
        : QTreeWidgetItem(), frame_info(frame_info),
          animation_info(animation_info) {

        // Compute name
        compute_name();
    }
    ~FrameTwi() {}

    void compute_name() {
        auto frame_name = "frame " + QString::number(frame_info->index);
        if (!frame_info->name.isEmpty())
            frame_name += " (" + frame_info->name + ")";
        setText(0, frame_name);
    }
};

#endif // __FRAME_TWI_H__