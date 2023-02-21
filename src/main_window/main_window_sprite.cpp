#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "util.h"
#include "gui/lwi/sprite_lwi.h"

// //////////////////////// //
// MAIN WINDOW SPRITE SLOTS //
// //////////////////////// //

void MainWindow::on_list_sprites_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    const auto sprite_lwi = dynamic_cast<SpriteLwi*>(current);
    if (sprite_lwi == nullptr) return;
    load_sprite(sprite_lwi->sprite);
}

void MainWindow::on_bt_add_sprite_clicked() {
    // Create new sprite
    auto new_sprite_ptr = _opd->add_new_sprite();

    // Add to list
    const auto sprite_lwi = new SpriteLwi(new_sprite_ptr);
    ui->list_sprites->addItem(sprite_lwi);

    // Set newest sprite as current
    ui->list_sprites->setCurrentItem(sprite_lwi);
}
void MainWindow::on_bt_remove_sprite_clicked() {
    check_if_valid(_current_sprite);

    // Get current lwi
    const auto sprite_lwi =
        dynamic_cast<SpriteLwi*>(ui->list_sprites->currentItem());
    if (sprite_lwi == nullptr) return;

    // Remove it
    _opd->sprites.erase(_current_sprite);
    ui->list_sprites->removeItemWidget(sprite_lwi);
    delete sprite_lwi;

    // If this is the last sprite
    if (_opd->sprites.size() == 0) { set_sprite_edit_enabled(false); }
    // TODO: Update animation page

    // Update indices
    ushort index = 0;
    for (auto& sprite : _opd->sprites)
        sprite.index = index++;
}

void MainWindow::on_bt_sprite_trim_clicked() {
    check_if_valid(_current_sprite);
}
void MainWindow::on_bt_merge_layers_clicked() {
    check_if_valid(_current_sprite);
}
void MainWindow::on_bt_save_sprite_clicked() {
    check_if_valid(_current_sprite);
}
void MainWindow::on_ch_background_enabled_toggled(bool new_value) {
    check_if_valid(_current_sprite);
    ui->gv_sprite->show_sprite(
        *_current_sprite,
        _current_sprite_palette,
        ui->ch_background_enabled->isChecked()
    );
}
void MainWindow::on_slider_transparency_valueChanged(int new_value) {
    check_if_valid(_current_sprite);
}
void MainWindow::on_spin_sprite_pos_x_valueChanged(int new_value) {
    check_if_valid(_current_sprite);
}
void MainWindow::on_spin_sprite_pos_y_valueChanged(int new_value) {
    check_if_valid(_current_sprite);
}
void MainWindow::on_spin_sprite_width_valueChanged(int new_value) {
    check_if_valid(_current_sprite);
    _current_sprite->width = new_value;
}
void MainWindow::on_spin_sprite_height_valueChanged(int new_value) {
    check_if_valid(_current_sprite);
    _current_sprite->height = new_value;
}
void MainWindow::on_cb_sprite_palette_currentIndexChanged(int new_index) {
    static bool in_loop = false;
    if (new_index == -1 || in_loop) return;
    const auto cb = ui->cb_sprite_palette;

    if (new_index == cb->count() - 1) {
        // Add new option picked, adding new palette

        // Update combo box
        in_loop = true;
        cb->insertItem(new_index, QString::number(new_index));
        in_loop = false;

        // Select it
        cb->setCurrentIndex(new_index);

        return;
    }
}

// -----------------------------------------------------------------------------
// Sprite palette buttons
// -----------------------------------------------------------------------------

#define button_click_method(i)                                                 \
    void MainWindow::on_bt_sprite_col_##i##_clicked() {                        \
        on_bt_sprite_col_clicked(ui->bt_sprite_col_##i);                       \
    }

button_click_method(0);
button_click_method(1);
button_click_method(2);
button_click_method(3);
button_click_method(4);
button_click_method(5);
button_click_method(6);
button_click_method(7);
button_click_method(8);
button_click_method(9);
button_click_method(A);
button_click_method(B);
button_click_method(C);
button_click_method(D);
button_click_method(E);
button_click_method(F);

// ////////////////////////////////// //
// MAIN WINDOW SPRITE PRIVATE METHODS //
// ////////////////////////////////// //

void MainWindow::load_sprites() {
    // Setup palettes
    for (auto i = 1; i < ui->cb_sprite_palette->count() - 1; i++)
        ui->cb_sprite_palette->removeItem(i);
    for (auto i = 1; i < _opd->palettes.size(); i++)
        ui->cb_sprite_palette->insertItem(i, QString::number(i));
    _current_sprite_palette = *_opd->palettes.begin();

    // Add all loaded sprites
    ForEach(sprite, _opd->sprites)
        ui->list_sprites->addItem(new SpriteLwi(sprite));

    // Select first
    ui->list_sprites->setCurrentRow(0);
}
void MainWindow::load_sprite(const SpritePtr sprite) {
    _current_sprite = sprite;

    // Update ui
    ui->gv_sprite->show_sprite(
        *_current_sprite,
        _current_sprite_palette,
        ui->ch_background_enabled->isChecked()
    );
    ui->spin_sprite_pos_x->setValue(0);
    ui->spin_sprite_pos_y->setValue(0);
    ui->spin_sprite_width->setValue(sprite->width);
    ui->spin_sprite_height->setValue(sprite->height);

    // Enable editing
    set_sprite_edit_enabled(true);
}
void MainWindow::clear_sprite() {
    _current_sprite = Invalid::sprite;

    // Update ui
    ui->gv_sprite->scene()->clear();
    ui->spin_sprite_pos_x->clear();
    ui->spin_sprite_pos_y->clear();
    ui->spin_sprite_width->clear();
    ui->spin_sprite_height->clear();

    // Disable editing
    set_sprite_edit_enabled(true);
}

void MainWindow::on_bt_sprite_col_clicked(PaletteButton* const button) {}

void MainWindow::set_sprite_edit_enabled(bool enabled) {
    ui->bt_remove_sprite->setEnabled(enabled);
    ui->ch_background_enabled->setEnabled(enabled);
    ui->slider_transparency->setEnabled(enabled);
    ui->bt_sprite_trim->setEnabled(enabled);
    ui->bt_merge_layers->setEnabled(enabled);
    ui->bt_save_sprite->setEnabled(enabled);
    ui->spin_sprite_pos_x->setEnabled(enabled);
    ui->spin_sprite_pos_y->setEnabled(enabled);
    ui->spin_sprite_width->setEnabled(enabled);
    ui->spin_sprite_height->setEnabled(enabled);
}