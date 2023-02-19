#include "main_window.h"
#include "../forms/ui_main_window.h"

#include "util.h"
#include "gui/frame_part_lwi.h"
#include "gui/new_part_dialog.h"

// //////////////////////////// //
// MAIN WINDOW FRAME PART SLOTS //
// //////////////////////////// //

void MainWindow::on_list_frame_parts_itemPressed(QListWidgetItem* current) {
    auto part_twi = dynamic_cast<FramePartLwi*>(current);
    if (part_twi == nullptr) return;
    load_frame_part(part_twi->frame_part);
}
void MainWindow::on_list_frame_parts_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    on_list_frame_parts_itemPressed(current);
}

void MainWindow::on_bt_add_frame_part_clicked() {
    check_if_valid(_current_frame);

    // Prompt to add new sprite
    NewPartDialog dialog { _opd };
    dialog.setModal(true);
    if (!dialog.exec()) return; // Rejected

    // Get selected sprite & palette
    const auto sprite  = dialog.selected_sprite;
    const auto palette = dialog.selected_palette;

    // Create new frame part
    _current_frame->parts.push_back(
        { (ushort) _current_frame->parts.size(), sprite, palette, 0, 0, 0 }
    );
    auto new_frame_part = _current_frame->parts.end();
    new_frame_part--;

    // Add to list
    const auto frame_part_lwi = new FramePartLwi(new_frame_part);
    ui->list_frame_parts->addItem(frame_part_lwi);

    // Select
    ui->list_frame_parts->setCurrentItem(frame_part_lwi);
}
void MainWindow::on_bt_remove_frame_part_clicked() {
    check_if_valid(_current_frame_part);
    const auto list = ui->list_frame_parts;

    // Get current part lwi
    const auto part_lwi = dynamic_cast<FramePartLwi*>(list->currentItem());
    if (part_lwi == nullptr) return;

    // Remove it
    _current_frame->parts.erase(_current_frame_part);
    list->removeItemWidget(part_lwi);
    delete part_lwi;

    // If this is last frame part clear ui
    if (_current_frame->parts.size() == 0) clear_frame_part();
    // Otherwise update arrows
    else set_frame_part_movement_enabled(true);

    // Update indices
    ushort index = 0;
    for (auto& part : _current_frame->parts)
        part.index = index++;

    // Redraw frame
    ui->gv_frame->show_frame(*_current_frame);
}
void MainWindow::on_bt_frame_part_up_clicked() {
    check_if_valid(_current_frame_part);
    const auto list = ui->list_frame_parts;

    // Get index
    const auto part_index = _current_frame_part->index;
    if (part_index == 0) return; // Cant go up further

    // Update ui list location
    const auto part_neighbour =
        dynamic_cast<FramePartLwi*>(list->takeItem(part_index - 1));
    list->insertItem(part_index, part_neighbour);

    // Update location in frame (frame part order)
    _current_frame->parts.splice(
        part_neighbour->frame_part, _current_frame->parts, _current_frame_part
    );

    // Update indices
    part_neighbour->frame_part->index = part_index;
    _current_frame_part->index        = part_index - 1;

    // Update arrows
    set_frame_part_movement_enabled(true);

    // Redraw frame
    ui->gv_frame->show_frame(*_current_frame);
}
void MainWindow::on_bt_frame_part_down_clicked() {
    check_if_valid(_current_frame_part);
    const auto list = ui->list_frame_parts;

    // Get index
    const auto part_index = _current_frame_part->index;
    if (part_index == _current_frame->parts.size() - 1)
        return; // Cant go down further

    // Update ui list location
    const auto part_neighbour =
        dynamic_cast<FramePartLwi*>(list->takeItem(part_index + 1));
    list->insertItem(part_index, part_neighbour);

    // Update location in frame (frame part order)
    _current_frame->parts.splice(
        _current_frame_part, _current_frame->parts, part_neighbour->frame_part
    );

    // Update indices
    part_neighbour->frame_part->index = part_index;
    _current_frame_part->index        = part_index + 1;

    // Update arrows
    set_frame_part_movement_enabled(true);

    // Redraw frame
    ui->gv_frame->show_frame(*_current_frame);
}

#define change_frame_part_value(attribute, new_value)                          \
    check_if_valid(_current_frame_part);                                       \
    _current_frame_part->attribute = new_value;                                \
    ui->gv_frame->show_frame(*_current_frame)

void MainWindow::on_spin_frame_part_off_x_valueChanged(int new_value) {
    change_frame_part_value(x_offset, new_value);
}
void MainWindow::on_spin_frame_part_off_y_valueChanged(int new_value) {
    change_frame_part_value(y_offset, new_value);
}
void MainWindow::on_ch_frame_part_flip_x_toggled(bool new_value) {
    check_if_valid(_current_frame_part);
    _current_frame_part->flip_mode &= 0b01;
    _current_frame_part->flip_mode |= new_value << 1;
    ui->gv_frame->show_frame(*_current_frame);
}
void MainWindow::on_ch_frame_part_flip_y_toggled(bool new_value) {
    check_if_valid(_current_frame_part);
    _current_frame_part->flip_mode &= 0b10;
    _current_frame_part->flip_mode |= (uchar) new_value;
    ui->gv_frame->show_frame(*_current_frame);
}
void MainWindow::on_cb_frame_part_color_set_currentIndexChanged(int new_index) {
    if (new_index < 0) return;
    auto new_palette = _opd->palettes.begin();
    std::advance(new_palette, new_index);
    change_frame_part_value(palette, new_palette);
    load_palette();
}

// -----------------------------------------------------------------------------
// Palette buttons
// -----------------------------------------------------------------------------

#define button_click_method(method, button)                                    \
    void MainWindow::method() { on_bt_frame_part_col_clicked(ui->button); }

button_click_method(on_bt_frame_part_col_0_clicked, bt_frame_part_col_0);
button_click_method(on_bt_frame_part_col_1_clicked, bt_frame_part_col_1);
button_click_method(on_bt_frame_part_col_2_clicked, bt_frame_part_col_2);
button_click_method(on_bt_frame_part_col_3_clicked, bt_frame_part_col_3);
button_click_method(on_bt_frame_part_col_4_clicked, bt_frame_part_col_4);
button_click_method(on_bt_frame_part_col_5_clicked, bt_frame_part_col_5);
button_click_method(on_bt_frame_part_col_6_clicked, bt_frame_part_col_6);
button_click_method(on_bt_frame_part_col_7_clicked, bt_frame_part_col_7);
button_click_method(on_bt_frame_part_col_8_clicked, bt_frame_part_col_8);
button_click_method(on_bt_frame_part_col_9_clicked, bt_frame_part_col_9);
button_click_method(on_bt_frame_part_col_A_clicked, bt_frame_part_col_A);
button_click_method(on_bt_frame_part_col_B_clicked, bt_frame_part_col_B);
button_click_method(on_bt_frame_part_col_C_clicked, bt_frame_part_col_C);
button_click_method(on_bt_frame_part_col_D_clicked, bt_frame_part_col_D);
button_click_method(on_bt_frame_part_col_E_clicked, bt_frame_part_col_E);
button_click_method(on_bt_frame_part_col_F_clicked, bt_frame_part_col_F);

// ////////////////////////////////////// //
// MAIN WINDOW FRAME PART PRIVATE METHODS //
// ////////////////////////////////////// //

void MainWindow::load_frame_parts() {
    check_if_valid(_current_frame);
    auto list = ui->list_frame_parts;

    // Add all parts
    list->clear();
    ForEach(part, _current_frame->parts) {
        auto part_lwi = new FramePartLwi(part);
        list->addItem(part_lwi);
    }

    // Select first
    if (list->count()) list->setCurrentRow(0);
}
void MainWindow::load_frame_part(const FramePartPtr frame_part) {
    _current_frame_part = frame_part;

    ui->spin_frame_part_off_x->setValue(frame_part->x_offset);
    ui->spin_frame_part_off_y->setValue(frame_part->y_offset);
    ui->ch_frame_part_flip_x->setChecked(frame_part->flip_mode & 0b10);
    ui->ch_frame_part_flip_y->setChecked(frame_part->flip_mode & 0b01);
    ui->cb_frame_part_color_set->setCurrentIndex(frame_part->palette->index);

    // Load palette colors
    load_palette();

    // Enable editing
    set_frame_part_edit_enabled(true);
}
void MainWindow::clear_frame_part() {
    _current_frame_part = Invalid::frame_part;

    ui->list_frame_parts->clear();
    ui->spin_frame_part_off_x->clear();
    ui->spin_frame_part_off_y->clear();
    ui->ch_frame_part_flip_x->setChecked(false);
    ui->ch_frame_part_flip_y->setChecked(false);
    ui->cb_frame_part_color_set->setCurrentIndex(0);

    // Load palette colors
    bt_frame_part_col_ALL(setStyleSheet(""));

    // Disable editing
    set_frame_part_edit_enabled(false);
}

void MainWindow::load_palette() {
    check_if_valid(_current_frame_part);
    _current_palette = *_current_frame_part->palette;
    bt_frame_part_col_ALL(set_color());
}

void MainWindow::on_bt_frame_part_col_clicked(PaletteButton* const button) {
    if (_current_palette.size == 0) return;

    // Get new color
    update_color(_current_palette[button->color_index]);

    // Set palette
    (*_current_frame_part->palette)[button->color_index] =
        _current_palette[button->color_index];

    // Setup button color
    button->set_color();

    // Redraw frame
    ui->gv_frame->show_frame(*_current_frame);
}

void MainWindow::set_frame_part_edit_enabled(bool enabled) {
    ui->bt_remove_frame_part->setEnabled(enabled);
    ui->bt_merge_frame_parts->setEnabled(enabled);
    ui->bt_edit_frame_part->setEnabled(enabled);
    ui->spin_frame_part_off_x->setEnabled(enabled);
    ui->spin_frame_part_off_y->setEnabled(enabled);
    ui->ch_frame_part_flip_x->setEnabled(enabled);
    ui->ch_frame_part_flip_y->setEnabled(enabled);
    ui->cb_frame_part_color_set->setEnabled(enabled);
    bt_frame_part_col_ALL(setEnabled(enabled));

    // Enable up / down arrows only if movement is possible
    set_frame_part_movement_enabled(enabled);
}

void MainWindow::set_frame_part_movement_enabled(bool enabled) {
    auto position  = ui->list_frame_parts->currentRow();
    bool not_first = position != 0;
    bool not_last  = position != ui->list_frame_parts->count() - 1;

    ui->bt_frame_part_up->setEnabled(enabled && not_first);
    ui->bt_frame_part_down->setEnabled(enabled && not_last);
}
