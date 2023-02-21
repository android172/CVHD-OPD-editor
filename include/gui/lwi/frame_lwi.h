#ifndef __FRAME_LWI_H__
#define __FRAME_LWI_H__

#include <QListWidgetItem>
#include "opd/frame.h"

class FrameLwi : public QListWidgetItem {
  public:
    const FramePtr frame;

    FrameLwi(const FramePtr frame) : QListWidgetItem(), frame(frame) {
        compute_name();
    }
    ~FrameLwi() {}

    void compute_name() {
        auto name = "Frame " + QString::number(frame->index);
        if (!frame->name.isEmpty()) name += " (" + frame->name + ")";
        setText(name);
    }
};

#endif // __FRAME_LWI_H__