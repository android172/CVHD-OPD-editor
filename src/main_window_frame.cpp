#include "main_window.h"
#include "ui_main_window.h"

#include "gui/frame_twi.h"

// /////////////////////// //
// MAIN WINDOW FRAME SLOTS //
// /////////////////////// //

void MainWindow::on_line_frame_name_textEdited(QString new_text) {
    if (_current_frame == FramePtr()) return;
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
    if (_current_frame == InvalidPtr::frame) return;
    _current_frame->x_offset = new_value;
}
void MainWindow::on_spin_frame_pos_y_valueChanged(int new_value) {
    if (_current_frame == InvalidPtr::frame) return;
    _current_frame->y_offset = new_value;
}
void MainWindow::on_spin_frame_off_x_valueChanged(int new_value) {
    if (_current_anim_frame == InvalidPtr::animation_frame) return;
    _current_anim_frame->x_offset = new_value;
}
void MainWindow::on_spin_frame_off_y_valueChanged(int new_value) {
    if (_current_anim_frame == InvalidPtr::animation_frame) return;
    _current_anim_frame->y_offset = new_value;
}
void MainWindow::on_spin_frame_delay_valueChanged(int new_value) {
    if (_current_anim_frame == InvalidPtr::animation_frame) return;
    _current_anim_frame->delay = new_value;
}