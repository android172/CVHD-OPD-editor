#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "util.h"
#include "gui/lwi/hitbox_lwi.h"

// //////////////////////// //
// MAIN WINDOW HITBOX SLOTS //
// //////////////////////// //

void MainWindow::on_list_hitboxes_itemPressed(QListWidgetItem* current) {
    const auto hitbox_lwi = dynamic_cast<HitBoxLwi*>(current);
    if (hitbox_lwi == nullptr) return;
    load_hitbox(hitbox_lwi->hitbox);
}
void MainWindow::on_list_hitboxes_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    on_list_hitboxes_itemPressed(current);
}

void MainWindow::on_bt_add_hitbox_clicked() {
    check_if_valid(_current_frame);
    const auto list = ui->list_hitboxes;

    // Add hitbox to the end of hitbox list
    auto new_hitbox = _opd->add_new_hitbox(_current_frame);

    // Create new LWI
    auto hitbox_lwi = new HitBoxLwi(new_hitbox);
    list->addItem(hitbox_lwi);

    // Select newly created hitbox
    list->setCurrentItem(hitbox_lwi);

    // Redraw frame
    redraw_frame();
}
void MainWindow::on_bt_remove_hitbox_clicked() {
    check_if_valid(_current_hitbox);
    const auto list = ui->list_hitboxes;

    // Get current hitbox lwi
    const auto hitbox_lwi = dynamic_cast<HitBoxLwi*>(list->currentItem());
    if (hitbox_lwi == nullptr) return;

    // Remove it
    _current_frame->hitboxes.erase(_current_hitbox);
    list->removeItemWidget(hitbox_lwi);
    delete hitbox_lwi;

    // If this is last hitbox clear ui
    if (_current_frame->hitboxes.size() == 0) clear_hitbox();
    // Otherwise update arrows
    else set_hitbox_movement_enabled(true);

    // Update indices
    ushort index = 0;
    for (auto& hitbox : _current_frame->hitboxes)
        hitbox.index = index++;

    // Redraw frame
    redraw_frame();
}
void MainWindow::on_bt_hitbox_up_clicked() {
    check_if_valid(_current_hitbox);
    const auto list = ui->list_hitboxes;

    // Get index
    const auto hitbox_index = _current_hitbox->index;
    if (hitbox_index == 0) return; // Cant go up further

    // Update ui list location
    const auto hitbox_neighbour =
        dynamic_cast<HitBoxLwi*>(list->takeItem(hitbox_index - 1));
    list->insertItem(hitbox_index, hitbox_neighbour);

    // Update location in frame's hitbox list
    _current_frame->hitboxes.splice(
        hitbox_neighbour->hitbox, _current_frame->hitboxes, _current_hitbox
    );

    // Update indices
    hitbox_neighbour->hitbox->index = hitbox_index;
    _current_hitbox->index          = hitbox_index - 1;

    // Update gv indices
    ui->gv_frame->current_index = hitbox_index - 1;

    // Update arrows
    set_hitbox_movement_enabled(true);

    // Stop playing animation
    if (_in_animation) stop_animation();

    // Redraw frame
    redraw_frame();
}
void MainWindow::on_bt_hitbox_down_clicked() {
    check_if_valid(_current_hitbox);
    const auto list = ui->list_hitboxes;

    // Get index
    const auto hitbox_index = _current_hitbox->index;
    if (hitbox_index == _current_frame->hitboxes.size() - 1)
        return; // Cant go down further

    // Update ui list location
    const auto hitbox_neighbour =
        dynamic_cast<HitBoxLwi*>(list->takeItem(hitbox_index + 1));
    list->insertItem(hitbox_index, hitbox_neighbour);

    // Update location in frame's hitbox list
    _current_frame->hitboxes.splice(
        _current_hitbox, _current_frame->hitboxes, hitbox_neighbour->hitbox
    );

    // Update indices
    hitbox_neighbour->hitbox->index = hitbox_index;
    _current_hitbox->index          = hitbox_index + 1;

    // Update gv indices
    ui->gv_frame->current_index = hitbox_index + 1;

    // Update arrows
    set_hitbox_movement_enabled(true);

    // Stop playing animation
    if (_in_animation) stop_animation();

    // Redraw frame
    redraw_frame();
}

#define change_hitbox_value(attribute, new_value)                              \
    check_if_valid(_current_hitbox);                                           \
    _current_hitbox->attribute = new_value;                                    \
    if (_in_animation) stop_animation();                                       \
    redraw_frame()

void MainWindow::on_spin_hitbox_pos_x_valueChanged(int new_value) {
    change_hitbox_value(x_position, new_value);
}
void MainWindow::on_spin_hitbox_pos_y_valueChanged(int new_value) {
    change_hitbox_value(y_position, new_value);
}
void MainWindow::on_spin_hitbox_width_valueChanged(int new_value) {
    change_hitbox_value(width, new_value);
}
void MainWindow::on_spin_hitbox_height_valueChanged(int new_value) {
    change_hitbox_value(height, new_value);
}

// ////////////////////////////////// //
// MAIN WINDOW HITBOX PRIVATE METHODS //
// ////////////////////////////////// //

void MainWindow::load_hitboxes() {
    check_if_valid(_current_frame);
    auto list = ui->list_hitboxes;

    // Add all hitboxes
    list->clear();
    ForEach(hitbox, _current_frame->hitboxes) {
        auto hitbox_lwi = new HitBoxLwi(hitbox);
        list->addItem(hitbox_lwi);
    }

    // Select first
    if (list->count()) list->setCurrentRow(0);
}
void MainWindow::load_hitbox(const HitBoxPtr hitbox) {
    _current_hitbox = hitbox;

    ui->spin_hitbox_pos_x->setValue(hitbox->x_position);
    ui->spin_hitbox_pos_y->setValue(hitbox->y_position);
    ui->spin_hitbox_width->setValue(hitbox->width);
    ui->spin_hitbox_height->setValue(hitbox->height);

    // Redraw
    ui->gv_frame->current_index = _current_hitbox->index;
    redraw_frame();

    // Enable editing
    set_hitbox_edit_enabled(true);

    // Stop playing animation
    if (_in_animation) stop_animation();
}
void MainWindow::clear_hitbox() {
    _current_hitbox = Invalid::hitbox;

    ui->list_hitboxes->clear();
    ui->spin_hitbox_pos_x->clear();
    ui->spin_hitbox_pos_y->clear();
    ui->spin_hitbox_width->clear();
    ui->spin_hitbox_height->clear();

    // Enable editing
    set_hitbox_edit_enabled(false);

    // Stop playing animation
    if (_in_animation) stop_animation();
}
void MainWindow::set_hitbox_edit_enabled(bool enabled) {
    ui->bt_remove_hitbox->setEnabled(enabled);
    ui->spin_hitbox_pos_x->setEnabled(enabled);
    ui->spin_hitbox_pos_y->setEnabled(enabled);
    ui->spin_hitbox_width->setEnabled(enabled);
    ui->spin_hitbox_height->setEnabled(enabled);

    // Enable up / down arrows only if movement is possible
    set_hitbox_movement_enabled(enabled);
}

void MainWindow::set_hitbox_movement_enabled(bool enabled) {
    auto position  = ui->list_hitboxes->currentRow();
    bool not_first = position != 0;
    bool not_last  = position != ui->list_hitboxes->count() - 1;

    ui->bt_hitbox_up->setEnabled(enabled && not_first);
    ui->bt_hitbox_down->setEnabled(enabled && not_last);
}