#include "gui/animation_tree.h"

// ///////////////////////////// //
// ANIMATION TREE PUBLIC METHODS //
// ///////////////////////////// //

AnimationTwi* AnimationTree::add_animation(AnimationPtr animation) {
    const auto animation_twi = new AnimationTwi(animation);
    addTopLevelItem(animation_twi);
    return animation_twi;
}

QTreeWidgetItem* AnimationTree::add_unused_section() {
    const auto unused_twi = new QTreeWidgetItem();
    unused_twi->setText(0, "unused");
    insertTopLevelItem(0, unused_twi);
    return unused_twi;
}

void AnimationTree::delete_animation(AnimationTwi* animation_twi) {
    if (animation_twi == nullptr) return;
    const auto animation_index = indexOfTopLevelItem(animation_twi);
    delete takeTopLevelItem(animation_index);
}

QTreeWidgetItem* AnimationTree::get_unused_section() const {
    const auto current_twi = topLevelItem(0);
    if (dynamic_cast<AnimationTwi*>(current_twi) != nullptr) return nullptr;
    return current_twi;
}
AnimationTwi* AnimationTree::get_animation_section(const int i) const {
    return dynamic_cast<AnimationTwi*>(topLevelItem(i));
}

AnimationTwi* AnimationTree::get_current_animation() const {
    const auto current_twi = currentItem();
    if (current_twi == nullptr) return nullptr;
    const auto top_level_twi =
        (current_twi->parent()) ? current_twi->parent() : current_twi;
    return dynamic_cast<AnimationTwi*>(top_level_twi);
}

FrameTwi* AnimationTree::get_current_frame() const {
    return dynamic_cast<FrameTwi*>(currentItem());
}