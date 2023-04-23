#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "gui/twi/frame_twi.h"
#include "gui/twi/animation_twi.h"
#include "util.h"

#include <QTimer>

// /////////////////////////// //
// MAIN WINDOW ANIMATION SLOTS //
// /////////////////////////// //

void MainWindow::on_tree_animations_itemPressed(QTreeWidgetItem* current, int) {
    if (current == nullptr) return;

    // Get current frame twi and its corresponding animation (if applicable)
    AnimationTwi* animation_twi = ui->tree_animations->get_current_animation();
    FrameTwi*     frame_twi     = ui->tree_animations->get_current_frame();
    if (!frame_twi && animation_twi) frame_twi = animation_twi->get_frame(0);

    // Load animation
    if (animation_twi) load_animation(animation_twi->animation);
    else clear_animation();

    // Load frame
    if (frame_twi) load_frame(frame_twi->frame_info, frame_twi->animation_info);
    else clear_frame();
}

void MainWindow::on_tree_animations_currentItemChanged(
    QTreeWidgetItem* current, QTreeWidgetItem* previous
) {
    on_tree_animations_itemPressed(current, 0);
}

void MainWindow::on_bt_add_animation_clicked() {
    // === UPDATE VALUE ===
    save_previous_state();
    // Create new animation
    const auto new_animation = _opd->add_new_animation();

    // === UPDATE UI ===
    // Create new animation TWI and add it to tree
    const auto animation_twi =
        ui->tree_animations->add_animation(new_animation);

    // Select newest animation
    ui->tree_animations->setCurrentItem(animation_twi);
}

void MainWindow::on_bt_remove_animation_clicked() {
    check_if_valid(_current_animation);

    // === UPDATE VALUE ===
    save_previous_state();
    // Remove animation
    for (const auto frame : _current_animation->frames)
        frame.data->uses--;
    _opd->animations.erase(_current_animation);

    // Update indices
    ushort index = 0;
    for (auto& animation : _opd->animations)
        animation.index = index++;

    // === UPDATE UI ===
    auto tree = ui->tree_animations;

    // Get current animation
    const auto animation_twi = tree->get_current_animation();

    // Remove all of its frames
    for (auto i = animation_twi->childCount() - 1; i >= 0; i--) {
        const auto frame_twi =
            dynamic_cast<FrameTwi*>(animation_twi->takeChild(0));

        // If this was the last animation that used this frame move it to
        // unused section
        if (frame_twi->frame_info->uses == 0) {
            auto unused_twi = tree->get_unused_section();

            // If this is the first unused frame add unused section
            if (unused_twi == nullptr) unused_twi = tree->add_unused_section();

            frame_twi->animation_info = Invalid::animation_frame;
            unused_twi->addChild(frame_twi);
        } else delete frame_twi;
    }

    // Remove the animation
    tree->delete_animation(animation_twi);

    // Inform rest
    clear_animation();
    clear_frame();
}

void MainWindow::on_line_animation_name_textEdited(QString new_text) {
    check_if_valid(_current_animation);

    // === UPDATE VALUE ===
    save_previous_state();
    _current_animation->name = new_text;

    // === UPDATE UI ===
    const auto animation_twi = ui->tree_animations->get_current_animation();
    if (animation_twi == nullptr) return;
    animation_twi->compute_name();
}

void MainWindow::on_slider_animation_speed_valueChanged(int new_value) {
    ui->label_animation_speed->setText(QString::number(new_value) + "%");
}

void MainWindow::on_bt_play_animation_clicked() {
    if (ui->bt_play_animation->isChecked()) {
        // Get current animation TWI
        const auto animation_twi = ui->tree_animations->get_current_animation();
        if (!animation_twi) return;

        // Display first frame immediately
        _in_animation = true;
        animate_frame(*animation_twi->animation, 0);
    } else {
        // This will stop the animation
        on_tree_animations_itemPressed(ui->tree_animations->currentItem(), 0);
    }
}

// ///////////////////////////////////// //
// MAIN WINDOW ANIMATION PRIVATE METHODS //
// ///////////////////////////////////// //

void MainWindow::load_animations() {
    auto tree = ui->tree_animations;
    tree->clear();

    // Add all animations from OPD
    ForEach(animation, _opd->animations) tree->add_animation(animation);

    // Add unused frames section if necessary
    if (_opd->frames.size() > 0) {
        const auto unused_twi = tree->add_unused_section();

        // Add all unused frames
        ForEach(frame, _opd->frames) {
            if (frame->uses == 0)
                unused_twi->addChild(
                    new FrameTwi(frame, Invalid::animation_frame)
                );
        }
    }

    // Select first item
    tree->setCurrentItem(tree->topLevelItem(0));
}

void MainWindow::load_animation(const AnimationPtr animation) {
    _current_animation = animation;

    change_ui(line_animation_name, setText(animation->name));

    set_animation_edit_enabled(true);

    // Stop playing animation
    if (_in_animation) stop_animation();
}
void MainWindow::clear_animation() {
    _current_animation = Invalid::animation;

    change_ui(line_animation_name, setText(""));

    set_animation_edit_enabled(false);

    // Stop playing animation
    if (_in_animation) stop_animation();
}

void MainWindow::animate_frame(const Animation& animation, ushort frame_count) {
    if (!_in_animation) return;

    // Current frame
    const auto frame = get_val_at(animation.frames, frame_count);

    // Show current frame
    if (ui->tool_box_frame->currentIndex() == 0)
        ui->gv_frame->show_frame(*frame.data, _current_frame_part->index);
    else ui->gv_frame->show_frame(*frame.data, _current_hitbox->index, true);

    // Update frame count
    frame_count++;
    const bool last_frame = frame_count >= animation.frames.size();
    if (last_frame) frame_count = 0;

    // Compute delay
    const auto delay = // delay[ns] = (10*frame_delay[ms]) * (100/multiplier[%])
        1000.0f * frame.delay / ui->slider_animation_speed->value();

    // Show next frame after timeout (if needed)
    if (ui->ch_repeat_animation->isChecked() || last_frame == false)
        QTimer::singleShot(delay, [&, frame_count]() {
            animate_frame(animation, frame_count);
        });
    else QTimer::singleShot(delay, [&]() { ui->bt_play_animation->click(); });
}

void MainWindow::stop_animation() {
    _in_animation = false;
    if (ui->bt_play_animation->isChecked())
        ui->bt_play_animation->setChecked(false);
}

void MainWindow::set_animation_edit_enabled(bool enabled) {
    ui->line_animation_name->setEnabled(enabled);
    ui->ch_repeat_animation->setEnabled(enabled);
    ui->slider_animation_speed->setEnabled(enabled);
    ui->bt_play_animation->setEnabled(enabled);
}