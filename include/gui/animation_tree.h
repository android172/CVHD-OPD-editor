#ifndef __ANIMATION_TREE_H__
#define __ANIMATION_TREE_H__

#include <QTreeWidget>

#include "twi/animation_twi.h"
#include "twi/frame_twi.h"

class AnimationTree : public QTreeWidget {
    Q_OBJECT
  public:
    using QTreeWidget::QTreeWidget;

    AnimationTwi*    add_animation(AnimationPtr animation);
    QTreeWidgetItem* add_unused_section();

    void delete_animation(AnimationTwi* animation_twi);

    AnimationTwi*    get_current_animation();
    QTreeWidgetItem* get_unused_section();
    FrameTwi*        get_current_frame();
};

#endif // __ANIMATION_TREE_H__