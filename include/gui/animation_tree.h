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

    QTreeWidgetItem* get_unused_section() const;
    AnimationTwi*    get_animation_section(const int i) const;

    AnimationTwi* get_current_animation() const;
    FrameTwi*     get_current_frame() const;
};

#endif // __ANIMATION_TREE_H__