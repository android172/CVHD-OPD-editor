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
    auto is_frame = current->parent() != nullptr;

    AnimationTwi* animation_twi = nullptr;
    FrameTwi*     frame_twi     = nullptr;
    if (is_frame) {
        animation_twi = dynamic_cast<AnimationTwi*>(current->parent());
        frame_twi     = dynamic_cast<FrameTwi*>(current);
    } else {
        animation_twi = dynamic_cast<AnimationTwi*>(current);
        if (animation_twi && animation_twi->animation->frames.size() > 0)
            frame_twi = dynamic_cast<FrameTwi*>(animation_twi->child(0));
    }

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
    // Create new animation
    auto new_animation = _opd->add_new_animation();

    // Create new animation TWI and add it to tree
    const auto animation_twi =
        ui->tree_animations->add_animation(new_animation);

    // Select newest animation
    ui->tree_animations->setCurrentItem(animation_twi);
}

void MainWindow::on_bt_remove_animation_clicked() {
    check_if_valid(_current_animation);
    auto tree = ui->tree_animations;

    // Cast current animation (if exist)
    const auto animation_twi = tree->get_current_animation();
    if (animation_twi == nullptr) return;

    // Remove all of its frames
    for (auto i = animation_twi->childCount() - 1; i >= 0; i--) {
        auto frame_twi = dynamic_cast<FrameTwi*>(animation_twi->takeChild(0));
        frame_twi->frame_info->uses--;

        // If this was the last animation that used this frame move it to
        // unused section
        if (frame_twi->frame_info->uses == 0) {
            auto unused_twi = tree->get_unused_section();

            // If this is the first unused frame add unused section
            if (unused_twi == nullptr) unused_twi = tree->add_unused_section();

            frame_twi->animation_info = Invalid::animation_frame;
            unused_twi->addChild(frame_twi);
        }
    }

    // Remove it
    _opd->animations.erase(_current_animation);
    tree->delete_animation(animation_twi);

    // Update indices
    ushort index = 0;
    for (auto& animation : _opd->animations)
        animation.index = index++;

    // Inform rest
    clear_animation();
    clear_frame();
}

void MainWindow::on_line_animation_name_textEdited(QString new_text) {
    if (_current_animation->index == Invalid::index) return;
    _current_animation->name = new_text;

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
        stop_animation();
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
    ui->line_animation_name->setText(animation->name);
    set_animation_edit_enabled(true);
}
void MainWindow::clear_animation() {
    _current_animation = Invalid::animation;
    ui->line_animation_name->setText("");
    set_animation_edit_enabled(false);
}

void MainWindow::animate_frame(const Animation& animation, ushort frame_count) {
    if (!_in_animation) return;

    // Current frame
    const auto frame = get_val_at(animation.frames, frame_count);

    // Show current frame
    ui->gv_frame->show_frame(*frame.data);

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
    on_tree_animations_itemPressed(ui->tree_animations->currentItem(), 0);
}

void MainWindow::set_animation_edit_enabled(bool enabled) {
    ui->line_animation_name->setEnabled(enabled);
    ui->ch_repeat_animation->setEnabled(enabled);
    ui->slider_animation_speed->setEnabled(enabled);
    ui->bt_play_animation->setEnabled(enabled);
}