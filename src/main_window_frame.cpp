#include "main_window.h"
#include "../forms/ui_main_window.h"

#include "gui/animation_twi.h"
#include "gui/frame_twi.h"
#include "gui/new_frame_dialog.h"
#include "util.h"

// /////////////////////// //
// MAIN WINDOW FRAME SLOTS //
// /////////////////////// //

#include <QDebug>

void MainWindow::on_bt_add_frame_clicked() {
    const auto tree = ui->tree_animations;

    // Get current top level twi
    auto current_twi = tree->currentItem();
    if (current_twi == nullptr) return;
    if (current_twi->parent()) current_twi = current_twi->parent();

    // Get current animation twi
    const auto animation_twi = dynamic_cast<AnimationTwi*>(current_twi);

    // If unused section is selected then new frame is created be default
    if (animation_twi == nullptr) {
        // Create new frame
        _opd->frames.push_back(
            { (ushort) _opd->frames.size(), "", 0, 0, {}, {} }
        );
        auto new_frame = _opd->frames.end();
        new_frame--;

        // Create new twi for this frame
        const auto frame_twi =
            new FrameTwi(new_frame, Invalid::animation_frame);
        current_twi->addChild(frame_twi);

        // Set it as current
        tree->setCurrentItem(frame_twi);
    }
    // Otherwise we prompt the user for further instructions
    else {
        // Ask for new frame
        NewFrameDialog dialog { _opd };
        dialog.setModal(true);
        const auto result = dialog.exec();

        // Canceled
        if (result == 0) return;

        // Get selected frame
        auto frame = dialog.selected_frame;

        // If an unused (previously existing) frame is added we need to remove
        // it from the unused section
        if (result == 1 && frame->uses == 0) {
            const auto unused_twi = tree->topLevelItem(0);

            // Find selected frame twi
            FrameTwi* selected_frame_twi = nullptr;
            for (auto i = 0; i < unused_twi->childCount(); i++) {
                auto frame_twi = dynamic_cast<FrameTwi*>(unused_twi->child(i));
                if (frame_twi->frame_info->index == frame->index) {
                    selected_frame_twi = frame_twi;
                    break;
                }
            }

            // If selected frame was not found something is fishy...
            if (selected_frame_twi == nullptr) return;

            // Otherwise remove it
            unused_twi->removeChild(selected_frame_twi);
            delete selected_frame_twi;
        }

        // Add as animation frame
        auto& frames = animation_twi->animation->frames;
        frames.push_back({ (ushort) frames.size(), frame, 0, 0, 0 });
        auto animation_frame = frames.end();
        animation_frame--;
        frame->uses++;

        // Add to tree
        const auto frame_twi = new FrameTwi(frame, animation_frame);
        animation_twi->addChild(frame_twi);

        // Set as current
        tree->setCurrentItem(frame_twi);
    }
}
void MainWindow::on_bt_remove_frame_clicked() {
    check_if_valid(_current_frame);
    auto tree = ui->tree_animations;

    // Get current frame
    const auto frame_twi = dynamic_cast<FrameTwi*>(tree->currentItem());
    if (frame_twi == nullptr) return;

    // If frame is used we just remove it from animation
    if (_current_frame->uses) {
        auto animation_twi = dynamic_cast<AnimationTwi*>(frame_twi->parent());
        if (!animation_twi) return;

        // To avoid invoking on_item_changed we will change the current tree
        // focus item to the parent animation (on_item_changed is only invoked
        // if  currently focused item is removed)
        if (animation_twi->childCount() == 1)
            tree->setCurrentItem(animation_twi);

        // Remove animation frame
        _current_animation->frames.erase(_current_anim_frame);
        _current_frame->uses--;
        animation_twi->removeChild(frame_twi); // This invokes on_item_changed

        // Update indices
        ushort index = 0;
        for (auto& frame : _current_animation->frames)
            frame.index = index++;

        // Clear frame view if animation has none left
        if (animation_twi->childCount() == 0) clear_frame();
        // Otherwise update arrows
        else set_frame_movement_enabled(true);

        // If this was the last animation that used this frame move it to
        // unused section
        if (frame_twi->frame_info->uses == 0) {
            auto unused_twi = tree->topLevelItem(0);
            // Create unused frames section if needed
            if (dynamic_cast<AnimationTwi*>(unused_twi) != nullptr) {
                unused_twi = new QTreeWidgetItem();
                unused_twi->setText(0, "unused");
                tree->insertTopLevelItem(0, unused_twi);
            }

            frame_twi->animation_info = Invalid::animation_frame;
            unused_twi->addChild(frame_twi);
        }
    }
    // Otherwise we remove it completely
    else {
        _opd->frames.erase(_current_frame);

        auto unused_twi = frame_twi->parent();
        unused_twi->removeChild(frame_twi); // This invokes on_item_changed
        delete frame_twi;

        // Update indices
        ushort index = 0;
        for (auto& frame : _opd->frames)
            frame.index = index++;

        // If unused section has no more frames we delete it too
        if (unused_twi->childCount() == 0) delete tree->takeTopLevelItem(0);
    }
}
void MainWindow::on_bt_frame_up_clicked() {
    check_if_valid(_current_frame);

    // Get current frame
    const auto frame_twi =
        dynamic_cast<FrameTwi*>(ui->tree_animations->currentItem());
    if (frame_twi == nullptr) return;

    // Get parent
    const auto parent_twi = frame_twi->parent();

    // Get frame index
    const auto frame_index = parent_twi->indexOfChild(frame_twi);
    if (frame_index == 0) return; // Cant go up further

    // Update location in tree
    const auto frame_neighbour =
        dynamic_cast<FrameTwi*>(parent_twi->takeChild(frame_index - 1));
    parent_twi->insertChild(frame_index, frame_neighbour);

    // Update location in animation (if applicable)
    if (_current_animation->index != Invalid::index) {
        _current_animation->frames.splice(
            frame_neighbour->animation_info,
            _current_animation->frames,
            frame_twi->animation_info
        );
    }

    // Update arrows
    set_frame_movement_enabled(true);

    // Update indices
    frame_twi->animation_info->index       = frame_index - 1;
    frame_neighbour->animation_info->index = frame_index;
}
void MainWindow::on_bt_frame_down_clicked() {
    check_if_valid(_current_frame);

    // Get current frame
    const auto frame_twi =
        dynamic_cast<FrameTwi*>(ui->tree_animations->currentItem());
    if (frame_twi == nullptr) return;

    // Get parent
    auto parent_twi = frame_twi->parent();

    // Get frame index
    const auto frame_index = parent_twi->indexOfChild(frame_twi);
    if (frame_index == parent_twi->childCount() - 1)
        return; // Cant go down further

    // Update location in tree
    const auto frame_neighbour =
        dynamic_cast<FrameTwi*>(parent_twi->takeChild(frame_index + 1));
    parent_twi->insertChild(frame_index, frame_neighbour);

    // Update location in animation (if applicable)
    if (_current_animation->index != Invalid::index) {
        _current_animation->frames.splice(
            frame_twi->animation_info,
            _current_animation->frames,
            frame_neighbour->animation_info
        );
    }

    // Update arrows
    set_frame_movement_enabled(true);

    // Update indices
    frame_twi->animation_info->index       = frame_index + 1;
    frame_neighbour->animation_info->index = frame_index;
}

void MainWindow::on_tool_box_frame_currentChanged(int current) {
    ui->gv_frame->hitbox_visible = current == 1;
    check_if_valid(_current_frame);
    ui->gv_frame->show_frame(*_current_frame);
}

void MainWindow::on_line_frame_name_textEdited(QString new_text) {
    check_if_valid(_current_frame);
    _current_frame->name = new_text;

    for (auto i = 0; i < ui->tree_animations->topLevelItemCount(); i++) {
        auto animation = ui->tree_animations->topLevelItem(i);
        for (auto j = 0; j < animation->childCount(); j++) {
            auto frame_twi = dynamic_cast<FrameTwi*>(animation->child(j));
            frame_twi->compute_name();
        }
    }
}

void MainWindow::on_spin_frame_pos_x_valueChanged(int new_value) {
    check_if_valid(_current_frame);
    _current_frame->x_offset = new_value;
}
void MainWindow::on_spin_frame_pos_y_valueChanged(int new_value) {
    check_if_valid(_current_frame);
    _current_frame->y_offset = new_value;
}
void MainWindow::on_spin_frame_off_x_valueChanged(int new_value) {
    check_if_valid(_current_anim_frame);
    _current_anim_frame->x_offset = new_value;
}
void MainWindow::on_spin_frame_off_y_valueChanged(int new_value) {
    check_if_valid(_current_anim_frame);
    _current_anim_frame->y_offset = new_value;
}
void MainWindow::on_spin_frame_delay_valueChanged(int new_value) {
    check_if_valid(_current_anim_frame);
    _current_anim_frame->delay = new_value;
}

// ///////////////////////////////// //
// MAIN WINDOW FRAME PRIVATE METHODS //
// ///////////////////////////////// //

void MainWindow::load_frame(
    const FramePtr frame_info_in, const AnimationFramePtr animation_info_in
) {
    // Update state
    _current_frame      = frame_info_in;
    _current_anim_frame = animation_info_in;

    // Update ui
    auto animation_info = Animation::Frame {};
    if (_current_frame->uses > 0) animation_info = *animation_info_in;

    // Animation frame data (if in animation)
    ui->spin_frame_off_x->setValue(animation_info.x_offset);
    ui->spin_frame_off_y->setValue(animation_info.y_offset);
    ui->spin_frame_delay->setValue(animation_info.delay);

    // Frame data
    ui->gv_frame->show_frame(*_current_frame);
    ui->line_frame_name->setText(_current_frame->name);
    ui->spin_frame_pos_x->setValue(_current_frame->x_offset);
    ui->spin_frame_pos_y->setValue(_current_frame->y_offset);

    // Add parts & hitboxes
    if (_current_frame->parts.size()) load_frame_parts();
    else clear_frame_part();
    if (_current_frame->hitboxes.size()) load_hitboxes();
    else clear_hitbox();

    // Enable editing
    set_frame_edit_enabled(true);
}
void MainWindow::clear_frame() {
    _current_frame      = Invalid::frame;
    _current_anim_frame = Invalid::animation_frame;

    ui->gv_frame->scene()->clear();
    ui->line_frame_name->clear();
    ui->spin_frame_delay->clear();
    ui->spin_frame_pos_x->clear();
    ui->spin_frame_pos_y->clear();
    ui->spin_frame_off_x->clear();
    ui->spin_frame_off_y->clear();

    // Clear part & hitbox
    clear_frame_part();
    clear_hitbox();

    // Disable editing
    set_frame_edit_enabled(false);
}

void MainWindow::set_frame_edit_enabled(bool enabled) {
    ui->line_frame_name->setEnabled(enabled);
    ui->spin_frame_pos_x->setEnabled(enabled);
    ui->spin_frame_pos_y->setEnabled(enabled);
    ui->bt_remove_frame->setEnabled(enabled);

    // Part & hitbox list can be added if frame editing is disabled
    ui->bt_add_hitbox->setEnabled(enabled);
    ui->bt_add_frame_part->setEnabled(enabled);

    // Enable up / down arrows only if movement is possible
    set_frame_movement_enabled(enabled);

    // Enable animation specific settings only if in animation
    auto in_animation = _current_anim_frame->index != Invalid::index;
    ui->spin_frame_off_x->setEnabled(enabled && in_animation);
    ui->spin_frame_off_y->setEnabled(enabled && in_animation);
    ui->spin_frame_delay->setEnabled(enabled && in_animation);
}

void MainWindow::set_frame_movement_enabled(bool enabled) {
    bool not_first   = false;
    bool not_last    = false;
    auto current_twi = ui->tree_animations->currentItem();
    if (current_twi && current_twi->parent()) {
        auto parent_twi = current_twi->parent();
        auto position   = parent_twi->indexOfChild(current_twi);
        not_first       = position != 0;
        not_last        = position != parent_twi->childCount() - 1;
    }

    ui->bt_frame_up->setEnabled(enabled && not_first);
    ui->bt_frame_down->setEnabled(enabled && not_last);
}