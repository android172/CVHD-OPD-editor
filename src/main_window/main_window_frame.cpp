#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "gui/twi/animation_twi.h"
#include "gui/twi/frame_twi.h"
#include "gui/new_frame_dialog.h"
#include "util.h"

// /////////////////////// //
// MAIN WINDOW FRAME SLOTS //
// /////////////////////// //

void MainWindow::on_bt_add_frame_clicked() {
    const auto tree = ui->tree_animations;

    // Get current animation twi & unused twi (JIC)
    const auto animation_twi = tree->get_current_animation();
    const auto unused_twi    = tree->get_unused_section();

    // If current section is unused new frame is created by default
    if (animation_twi == nullptr) {
        // Create new frame
        auto new_frame = _opd->add_new_frame();

        // Create new twi for this frame
        const auto frame_twi =
            new FrameTwi(new_frame, Invalid::animation_frame);
        unused_twi->addChild(frame_twi);

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
        auto animation_frame =
            _opd->add_new_animation_frame(animation_twi->animation, frame);
        frame->uses++;

        // Add to tree
        const auto frame_twi = animation_twi->add_frame(animation_frame);

        // Set as current
        tree->setCurrentItem(frame_twi);
    }
}
void MainWindow::on_bt_remove_frame_clicked() {
    check_if_valid(_current_frame);
    const auto tree = ui->tree_animations;

    // Get current frame
    const auto frame_twi = tree->get_current_frame();
    if (frame_twi == nullptr) return;

    // If frame is used we just remove it from animation
    if (_current_frame->uses) {
        const auto animation_twi =
            dynamic_cast<AnimationTwi*>(frame_twi->parent());
        if (!animation_twi) return;

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
            auto unused_twi = tree->get_unused_section();

            // Create unused frames section if needed
            if (unused_twi == nullptr) unused_twi = tree->add_unused_section();

            frame_twi->animation_info = Invalid::animation_frame;
            unused_twi->addChild(frame_twi);
        }
    }
    // Otherwise we remove it completely
    else {
        // Remove sprite uses
        for (auto& part : _current_frame->parts)
            part.sprite->uses--;

        // Remove frame itself
        _opd->frames.erase(_current_frame);

        const auto unused_twi = frame_twi->parent();
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
    const auto frame_twi = ui->tree_animations->get_current_frame();
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
    const auto frame_twi = ui->tree_animations->get_current_frame();
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

    if (current == 0) ui->gv_frame->current_index = _current_frame_part->index;
    else ui->gv_frame->current_index = _current_hitbox->index;

    // Stop playing animation
    if (_in_animation) stop_animation();

    redraw_frame();
}

void MainWindow::on_line_frame_name_textEdited(QString new_text) {
    check_if_valid(_current_frame);
    _current_frame->name = new_text;

    const auto frame_twi = ui->tree_animations->get_current_frame();
    if (frame_twi == nullptr) return;
    frame_twi->compute_name();
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
    redraw_frame();
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

    // Stop playing animation
    if (_in_animation) stop_animation();
}
void MainWindow::clear_frame() {
    _current_frame      = Invalid::frame;
    _current_anim_frame = Invalid::animation_frame;

    ui->gv_frame->clear();
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

    // Stop playing animation
    if (_in_animation) stop_animation();
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

void MainWindow::on_activate_frame_move_mode() {
    ui->gv_frame->activate_move([=](short dx, short dy) {
        if (ui->tool_box_frame->currentIndex() == 0) {
            check_if_valid(_current_frame_part);

            // Change values
            _current_frame_part->x_offset += dx;
            _current_frame_part->y_offset += dy;

            // Change their display
            ui->spin_frame_part_off_x->setValue(
                ui->spin_frame_part_off_x->value() + dx
            );
            ui->spin_frame_part_off_y->setValue(
                ui->spin_frame_part_off_y->value() + dy
            );
        } else {
            check_if_valid(_current_hitbox);

            // Change values
            _current_hitbox->x_position += dx;
            _current_hitbox->y_position += dy;

            // Change their display
            ui->spin_hitbox_pos_x->setValue(
                ui->spin_hitbox_pos_x->value() + dx
            );
            ui->spin_hitbox_pos_y->setValue(
                ui->spin_hitbox_pos_y->value() + dy
            );
        }
    });
}

void MainWindow::redraw_frame() { ui->gv_frame->show_frame(*_current_frame); }