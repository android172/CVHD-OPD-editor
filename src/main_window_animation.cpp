#include "main_window.h"
#include "ui_main_window.h"

#include "gui/frame_twi.h"
#include "gui/animation_twi.h"
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
        if (animation_twi && animation_twi->childCount() > 0)
            frame_twi = dynamic_cast<FrameTwi*>(animation_twi->child(0));
    }

    // Load frame
    if (frame_twi) load_frame(frame_twi->frame_info, frame_twi->animation_info);
    else clear_frame();

    // Load animation
    if (animation_twi) load_animation(animation_twi->animation);
    else clear_animation();
}

void MainWindow::on_tree_animations_currentItemChanged(
    QTreeWidgetItem* current, QTreeWidgetItem* previous
) {
    on_tree_animations_itemPressed(current, 0);
}

void MainWindow::on_bt_add_animation_clicked() {
    // Create new animation
    _opd->animations.push_back({ (uchar) _opd->animations.size(), "", {} });
    auto new_animation = _opd->animations.end() - 1;

    // Create new animation TWI and add it to tree
    const auto animation_twi = new AnimationTwi(new_animation);
    ui->tree_animations->addTopLevelItem(animation_twi);

    // Select newest animation
    ui->tree_animations->setCurrentItem(animation_twi);
}

void MainWindow::on_bt_remove_animation_clicked() {
    // Get current animation
    const auto animation_twi =
        dynamic_cast<AnimationTwi*>(ui->tree_animations->currentItem());
    if (animation_twi == nullptr) return;
    const auto anim_i = ui->tree_animations->indexOfTopLevelItem(animation_twi);

    // Remove it
    delete ui->tree_animations->takeTopLevelItem(anim_i);
    _opd->animations.erase(_current_animation);

    // Inform rest
    clear_animation();
    clear_frame();
}

void MainWindow::on_line_animation_name_textEdited(QString new_text) {
    if (_current_animation == InvalidPtr::animation) return;
    _current_animation->name = new_text;

    for (auto i = 0; i < ui->tree_animations->topLevelItemCount(); i++) {
        auto animation_twi =
            dynamic_cast<AnimationTwi*>(ui->tree_animations->topLevelItem(i));
        if (animation_twi) animation_twi->compute_name();
    }
}

void MainWindow::on_slider_animation_speed_valueChanged(int new_value) {
    ui->label_animation_speed->setText(QString::number(new_value) + "%");
}

void MainWindow::on_bt_play_animation_clicked() {
    if (ui->bt_play_animation->isChecked()) {
        auto current_twi = ui->tree_animations->currentItem();
        if (current_twi == nullptr) return;
        if (current_twi->parent() != nullptr)
            current_twi = current_twi->parent();

        // Get current animation TWI
        const auto animation_twi = dynamic_cast<AnimationTwi*>(current_twi);
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
    ForEach(_opd->animations, animation) {
        // Create animation TWI
        auto animation_twi = new AnimationTwi(animation);

        // Add frames for this animation
        ForEach(animation->frames, frame) {
            animation_twi->addChild(new FrameTwi(frame->data, frame));
        }

        tree->addTopLevelItem(animation_twi);
    }

    // Add unused frames section
    // Create unused TWI
    QString name       = "unused";
    auto    unused_twi = new QTreeWidgetItem();
    unused_twi->setText(0, name);

    // Add all unused frames
    ForEach(_opd->frames, frame) {
        if (frame->used == false)
            unused_twi->addChild(
                new FrameTwi(frame, InvalidPtr::animation_frame)
            );
    }

    if (unused_twi->childCount()) tree->insertTopLevelItem(0, unused_twi);
}

#include <QDebug>

void MainWindow::load_animation(const AnimationPtr animation) {
    _current_animation = animation;
    qDebug() << (size_t) ui->line_animation_name;
    qDebug() << (size_t) & *animation;
    qDebug() << (size_t) &animation->name;
    qDebug() << (size_t) animation->index;
    qDebug() << "=================================";
    ui->line_animation_name->setText(animation->name);
    ui->line_animation_name->setEnabled(true);
}
void MainWindow::clear_animation() {
    _current_animation = InvalidPtr::animation;
    ui->line_animation_name->setText("");
    ui->line_animation_name->setEnabled(false);
}

void MainWindow::load_frame(
    const FramePtr frame_info_in, const AnimationFramePtr animation_info_in
) {
    // Update state
    _current_frame      = frame_info_in;
    _current_anim_frame = animation_info_in;

    // Update ui
    auto animation_info = Animation::Frame { 0, 0, 0, 0 };
    if (_current_frame->used) animation_info = *animation_info_in;

    ui->gv_frame->show_frame(*_current_frame);
    ui->line_frame_name->setText(_current_frame->name);
    ui->spin_frame_pos_x->setValue(_current_frame->x_offset);
    ui->spin_frame_pos_y->setValue(_current_frame->y_offset);
    ui->spin_frame_off_x->setValue(animation_info.x_offset);
    ui->spin_frame_off_y->setValue(animation_info.y_offset);
    ui->spin_frame_delay->setValue(animation_info.delay);
}
void MainWindow::clear_frame() {
    _current_frame      = InvalidPtr::frame;
    _current_anim_frame = InvalidPtr::animation_frame;

    ui->gv_frame->scene()->clear();
    ui->line_frame_name->clear();
    ui->spin_frame_delay->cleanText();
    ui->spin_frame_pos_x->cleanText();
    ui->spin_frame_pos_y->cleanText();
    ui->spin_frame_off_x->cleanText();
    ui->spin_frame_off_y->cleanText();
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
    if (ui->cb_repeat_animation->isChecked() || last_frame == false)
        QTimer::singleShot(delay, [&, frame_count]() {
            animate_frame(animation, frame_count);
        });
    else QTimer::singleShot(delay, [&]() { ui->bt_play_animation->click(); });
}

void MainWindow::stop_animation() {
    _in_animation = false;
    on_tree_animations_itemPressed(ui->tree_animations->currentItem(), 0);
}