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
    // === Get context ===
    const auto tree          = ui->tree_animations;
    const auto animation_twi = tree->get_current_animation();
    const auto unused_twi    = tree->get_unused_section();

    // Here code forks
    // For unused section
    if (animation_twi == nullptr) {
        // === UPDATE VALUE ===
        save_previous_state();
        auto new_frame = _opd->add_new_frame();

        // === UPDATE UI ===
        // Create new twi for this frame
        const auto frame_twi =
            new FrameTwi(new_frame, Invalid::animation_frame);
        unused_twi->addChild(frame_twi);

        // Set it as current
        tree->setCurrentItem(frame_twi);
    }
    // For animation
    else {
        // Ask for new frame
        NewFrameDialog dialog { _opd };
        dialog.setModal(true);
        const auto result = dialog.exec();

        // Canceled
        if (result == 0) return;

        save_previous_state();
        for (const auto& frame : dialog.selected_frames) {
            // === UPDATE VALUE ===
            // Add as animation frame
            auto animation_frame =
                _opd->add_new_animation_frame(animation_twi->animation, frame);
            frame->uses++;

            // === UPDATE UI ===
            // Add to tree
            const auto frame_twi = animation_twi->add_frame(animation_frame);

            // If an unused (previously existing) frame is added we need to
            // remove it from the unused section
            if (result == 1 && frame->uses == 0) {
                // Find selected frame twi
                FrameTwi* selected_frame_twi = nullptr;
                for (auto i = 0; i < unused_twi->childCount(); i++) {
                    auto frame_twi =
                        dynamic_cast<FrameTwi*>(unused_twi->child(i));
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
        }

        // Set last added as current
        tree->setCurrentItem(
            animation_twi->child(animation_twi->childCount() - 1)
        );
    }
}
void MainWindow::on_bt_remove_frame_clicked() {
    check_if_valid(_current_frame);

    // === UPDATE VALUE ===
    save_previous_state();
    // If frame is used we just remove it from animation
    bool frame_is_used = _current_frame->uses != 0;
    if (frame_is_used) {
        // Remove animation frame
        _current_animation->frames.erase(_current_anim_frame);
        _current_frame->uses--;

        // Update indices
        ushort index = 0;
        for (auto& frame : _current_animation->frames)
            frame.index = index++;
    }
    // Otherwise we remove it completely
    else {
        // Remove sprite uses
        for (auto& part : _current_frame->parts)
            part.sprite->uses--;

        // Remove frame itself
        _opd->frames.erase(_current_frame);

        // Update indices
        ushort index = 0;
        for (auto& frame : _opd->frames)
            frame.index = index++;
    }

    // === UPDATE UI ===
    const auto tree = ui->tree_animations;

    // Get current frame
    const auto frame_twi = tree->get_current_frame();
    if (frame_twi == nullptr) return;

    // If frame is used we just remove it from animation
    if (frame_is_used) {
        const auto animation_twi = tree->get_current_animation();
        if (!animation_twi) return;

        // Remove animation frame
        animation_twi->removeChild(frame_twi); // This invokes on_item_changed

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
        } else delete frame_twi;
    }
    // Otherwise we remove it completely
    else {
        const auto unused_twi = frame_twi->parent();
        unused_twi->removeChild(frame_twi); // This invokes on_item_changed
        delete frame_twi;

        // If unused section has no more frames we delete it too
        if (unused_twi->childCount() == 0) delete tree->takeTopLevelItem(0);
    }
}
void MainWindow::on_bt_frame_up_clicked() {
    check_if_valid(_current_frame);

    // === UPDATE VALUE ===
    if (_current_animation->index != Invalid::index &&
        _current_anim_frame->index != 0) {
        save_previous_state();
        // Move frame up
        AnimationFramePtr frame_neighbour = _current_anim_frame;
        frame_neighbour--;
        _current_animation->frames.splice(
            frame_neighbour, _current_animation->frames, _current_anim_frame
        );

        // Update indices
        _current_anim_frame->index--;
        frame_neighbour->index++;
    }

    // === UPDATE UI ===
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

    // Update arrows
    set_frame_movement_enabled(true);
}
void MainWindow::on_bt_frame_down_clicked() {
    check_if_valid(_current_frame);

    // === UPDATE VALUE ===
    if (_current_animation->index != Invalid::index &&
        _current_anim_frame->index != _current_animation->frames.size()) {
        save_previous_state();
        // Move frame down
        AnimationFramePtr frame_neighbour = _current_anim_frame;
        frame_neighbour++;
        _current_animation->frames.splice(
            _current_anim_frame, _current_animation->frames, frame_neighbour
        );

        // Update indices
        _current_anim_frame->index++;
        frame_neighbour->index--;
    }

    // === UPDATE UI ===
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

    // Update arrows
    set_frame_movement_enabled(true);
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

    // === UPDATE VALUE ===
    save_previous_state();
    _current_frame->name = new_text;

    // === UPDATE UI ===
    const auto frame_twi = ui->tree_animations->get_current_frame();
    if (frame_twi == nullptr) return;
    frame_twi->compute_name();
}

void MainWindow::on_spin_frame_pos_x_valueChanged(int new_value) {
    check_if_valid(_current_frame);

    // === UPDATE VALUE ===
    save_previous_state();
    _current_frame->x_offset = new_value;
}
void MainWindow::on_spin_frame_pos_y_valueChanged(int new_value) {
    check_if_valid(_current_frame);

    // === UPDATE VALUE ===
    save_previous_state();
    _current_frame->y_offset = new_value;
}
void MainWindow::on_spin_frame_off_x_valueChanged(int new_value) {
    check_if_valid(_current_anim_frame);

    // === UPDATE VALUE ===
    save_previous_state();
    _current_anim_frame->x_offset = new_value;
}
void MainWindow::on_spin_frame_off_y_valueChanged(int new_value) {
    check_if_valid(_current_anim_frame);

    // === UPDATE VALUE ===
    save_previous_state();
    _current_anim_frame->y_offset = new_value;
}
void MainWindow::on_spin_frame_delay_valueChanged(int new_value) {
    check_if_valid(_current_anim_frame);

    // === UPDATE VALUE ===
    save_previous_state();
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

    _redrawing_frame = true;

    // Update ui
    auto animation_info = Animation::Frame {};
    if (_current_frame->uses > 0) animation_info = *animation_info_in;

    // Animation frame data (if in animation)
    change_ui(spin_frame_off_x, setValue(animation_info.x_offset));
    change_ui(spin_frame_off_y, setValue(animation_info.y_offset));
    change_ui(spin_frame_delay, setValue(animation_info.delay));

    // Frame data
    change_ui(line_frame_name, setText(_current_frame->name));
    change_ui(spin_frame_pos_x, setValue(_current_frame->x_offset));
    change_ui(spin_frame_pos_y, setValue(_current_frame->y_offset));

    // Add parts & hitboxes
    if (_current_frame->parts.size()) load_frame_parts();
    else clear_frame_part();
    if (_current_frame->hitboxes.size()) load_hitboxes();
    else clear_hitbox();

    // Redraw frame
    _redrawing_frame = false;
    redraw_frame();

    // Enable editing
    set_frame_edit_enabled(true);

    // Stop playing animation
    if (_in_animation) stop_animation();
}
void MainWindow::clear_frame() {
    _current_frame      = Invalid::frame;
    _current_anim_frame = Invalid::animation_frame;

    change_ui(gv_frame, clear());
    change_ui(line_frame_name, clear());
    change_ui(spin_frame_delay, clear());
    change_ui(spin_frame_pos_x, clear());
    change_ui(spin_frame_pos_y, clear());
    change_ui(spin_frame_off_x, clear());
    change_ui(spin_frame_off_y, clear());

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
    ui->gv_frame->activate_move([=](short dx, short dy, bool save) {
        if (ui->tool_box_frame->currentIndex() == 0) {
            check_if_valid(_current_frame_part);
            if (save) save_previous_state();

            // Change values
            _current_frame_part->x_offset += dx;
            _current_frame_part->y_offset += dy;

            // Change their display
            change_ui(
                spin_frame_part_off_x,
                setValue(ui->spin_frame_part_off_x->value() + dx)
            );
            change_ui(
                spin_frame_part_off_y,
                setValue(ui->spin_frame_part_off_y->value() + dy)
            );
        } else {
            check_if_valid(_current_hitbox);
            if (save) save_previous_state();

            // Change values
            _current_hitbox->x_position += dx;
            _current_hitbox->y_position += dy;

            // Change their display
            change_ui(
                spin_hitbox_pos_x, setValue(ui->spin_hitbox_pos_x->value() + dx)
            );
            change_ui(
                spin_hitbox_pos_y, setValue(ui->spin_hitbox_pos_y->value() + dy)
            );
        }
        redraw_frame();
    });
}

void MainWindow::redraw_frame() {
    if (_redrawing_frame) return;
    ui->gv_frame->show_frame(*_current_frame);
}