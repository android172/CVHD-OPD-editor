#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "util.h"
#include "gui/lwi/frame_part_lwi.h"
#include "gui/lwi/sprite_lwi.h"
#include "gui/new_part_dialog.h"

#include <QMessageBox>

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

    // Increment use count
    sprite->uses++;

    // Create new frame part
    const auto new_frame_part =
        _opd->add_new_frame_part(_current_frame, sprite, palette);

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

    // Decrement sprite use
    part_lwi->frame_part->sprite->uses--;

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
    redraw_frame();
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

    // Update gv indices
    ui->gv_frame->current_index = part_index - 1;

    // Update arrows
    set_frame_part_movement_enabled(true);

    // Redraw frame
    redraw_frame();

    // Stop playing animation
    if (_in_animation) stop_animation();
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

    // Update gv indices
    ui->gv_frame->current_index = part_index + 1;

    // Update arrows
    set_frame_part_movement_enabled(true);

    // Redraw frame
    redraw_frame();

    // Stop playing animation
    if (_in_animation) stop_animation();
}

void MainWindow::on_bt_merge_frame_parts_clicked() {
    check_if_valid(_current_frame_part);

    // Get selected frames
    const auto            selected_lwi = ui->list_frame_parts->selectedItems();
    QVector<FramePartPtr> used_frame_parts;
    used_frame_parts.reserve(selected_lwi.size());
    for (const auto& lwi : selected_lwi) {
        const auto frame_part_lwi = dynamic_cast<FramePartLwi*>(lwi);
        used_frame_parts.push_back(frame_part_lwi->frame_part);
    }
    if (used_frame_parts.size() < 2) return;

    // Compute min offsets
    auto min_x = SHRT_MAX;
    auto min_y = SHRT_MAX;

    // Also check if merger is possible
    const auto palette_used = used_frame_parts[0]->palette;
    for (const auto& frame_part : used_frame_parts) {
        // Fusion of frame parts with different pallets isn't done here
        if (palette_used != frame_part->palette) {
            QMessageBox::warning(
                this,
                "Invalid merger",
                "Frame part merger is only possible under a singular palette."
            );
            return;
        }
        if (frame_part->x_offset < min_x) min_x = frame_part->x_offset;
        if (frame_part->y_offset < min_y) min_y = frame_part->y_offset;
    }

    // Merge selected frame parts into a new sprite
    const auto new_sprite = _opd->add_new_sprite();
    new_sprite->from_frame_parts(used_frame_parts);
    new_sprite->uses++;

    // Remove now unused frame parts
    for (const auto& lwi : selected_lwi) {
        const auto frame_part_lwi = dynamic_cast<FramePartLwi*>(lwi);
        frame_part_lwi->frame_part->sprite->uses--;
        _current_frame->parts.erase(frame_part_lwi->frame_part);
        ui->list_frame_parts->removeItemWidget(frame_part_lwi);
        delete frame_part_lwi;
    }

    // Add new frame part
    const auto new_frame_part = _opd->add_new_frame_part(_current_frame);
    new_frame_part->sprite    = new_sprite;
    new_frame_part->palette   = palette_used;
    new_frame_part->x_offset  = min_x;
    new_frame_part->y_offset  = min_y;

    // Update indices
    ushort index = 0;
    for (auto& frame_part : _current_frame->parts)
        frame_part.index = index++;

    // Add new lwi
    const auto new_frame_part_lwi = new FramePartLwi(new_frame_part);
    ui->list_frame_parts->addItem(new_frame_part_lwi);

    // Set as current
    ui->list_frame_parts->setCurrentItem(new_frame_part_lwi);

    // Update in sprite list
    const auto new_sprite_lwi = new SpriteLwi(new_frame_part->sprite);
    ui->list_sprites->addItem(new_sprite_lwi);
}
void MainWindow::on_bt_edit_frame_part_clicked() {
    check_if_valid(_current_frame_part);
    ui->tab_main->setCurrentIndex(1);
    ui->list_sprites->setCurrentRow(_current_frame_part->sprite->index);

    // Stop playing animation
    if (_in_animation) stop_animation();
}

#define change_frame_part_value(attribute, new_value)                          \
    check_if_valid(_current_frame_part);                                       \
    _current_frame_part->attribute = new_value;                                \
    if (_in_animation) stop_animation();                                       \
    redraw_frame()

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

    // Stop playing animation
    if (_in_animation) stop_animation();

    redraw_frame();
}
void MainWindow::on_ch_frame_part_flip_y_toggled(bool new_value) {
    check_if_valid(_current_frame_part);
    _current_frame_part->flip_mode &= 0b10;
    _current_frame_part->flip_mode |= (uchar) new_value;

    // Stop playing animation
    if (_in_animation) stop_animation();

    redraw_frame();
}
void MainWindow::on_cb_frame_part_color_set_currentIndexChanged(int new_index) {
    if (new_index < 0) return;
    const auto new_palette = get_it_at(_opd->palettes, new_index);
    change_frame_part_value(palette, new_palette);
    load_frame_part_palette();
}

// -----------------------------------------------------------------------------
// Palette buttons
// -----------------------------------------------------------------------------

#define button_click_method(i, j)                                              \
    void MainWindow::on_bt_frame_part_col_##i##_clicked() {                    \
        on_bt_frame_part_col_clicked(ui->bt_frame_part_col_##i);               \
    }

APPLY_TO_COL(button_click_method);

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

    // Redraw
    ui->gv_frame->current_index = _current_frame_part->index;
    redraw_frame();

    // Load palette colors
    load_frame_part_palette();

    // Enable editing
    set_frame_part_edit_enabled(true);

    // Stop playing animation
    if (_in_animation) stop_animation();
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
    bt_frame_part_col_ALL(clear_color());

    // Disable editing
    set_frame_part_edit_enabled(false);

    // Stop playing animation
    if (_in_animation) stop_animation();
}

void MainWindow::load_frame_part_palette() {
    check_if_valid(_current_frame_part);
    _current_part_palette = *_current_frame_part->palette;
    bt_frame_part_col_ALL(clear_color());
    bt_frame_part_col_ALL(set_color());
}

void MainWindow::on_bt_frame_part_col_clicked(PaletteButton* const button) {
    if (button->color_index >= _current_part_palette.size) return;

    // Get new color
    auto color = _current_part_palette[button->color_index];
    prompt_color_dialog(color);

    // Update palettes
    update_palettes(_current_part_palette.index, button->color_index, color);

    // Stop playing animation
    if (_in_animation) stop_animation();
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
