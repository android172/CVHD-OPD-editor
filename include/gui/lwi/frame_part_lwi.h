#ifndef __FRAME_PART_LWI_H__
#define __FRAME_PART_LWI_H__

#include <QListWidgetItem>

#include "opd/frame.h"

class FramePartLwi : public QListWidgetItem {
  public:
    const FramePartPtr frame_part;

    FramePartLwi(const FramePartPtr frame_part)
        : QListWidgetItem(), frame_part(frame_part) {
        compute_name();
    };
    ~FramePartLwi() {}

    void compute_name() {
        setText("Sprite " + QString::number(frame_part->sprite->index));
    }

  private:
};

#endif // __FRAME_PART_LWI_H__