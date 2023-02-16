#ifndef __ANIMATION_TWI_H__
#define __ANIMATION_TWI_H__

#include <QTreeWidgetItem>

#include "opd_types.h"

class AnimationTwi : public QTreeWidgetItem {
  public:
    AnimationPtr animation;

    AnimationTwi(const AnimationPtr animation)
        : QTreeWidgetItem(), animation(animation) {
        compute_name();
    }
    ~AnimationTwi() {}

    void compute_name() {
        const auto animation_name =
            (animation->name.size())
                ? animation->name
                : "animation " + QString::number(animation->index);
        setText(0, animation_name);
    }
};

#endif // __ANIMATION_TWI_H__