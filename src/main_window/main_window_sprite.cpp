#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "util.h"
#include "gui/lwi/sprite_lwi.h"

#include <QFileDialog>

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
    // === UPDATE VALUE ===
    save_previous_state();
    // Create new sprite
    auto new_sprite_ptr = _opd->add_new_sprite();

    // === UPDATE UI ===
    // Add to list
    const auto sprite_lwi = new SpriteLwi(new_sprite_ptr);
    ui->list_sprites->addItem(sprite_lwi);

    // Set newest sprite as current
    ui->list_sprites->setCurrentItem(sprite_lwi);
}
void MainWindow::on_bt_remove_sprite_clicked() {
    check_if_valid(_current_sprite);

    // === UPDATE VALUE ===
    save_previous_state();
    // Remove frame parts which use this sprite
    bool current_frame_changed = false;
    for (auto& frame : _opd->frames) {
        auto change_occurred = false;

        // Remove parts
        for (auto part = frame.parts.begin(); part != frame.parts.end();) {
            if (part->sprite->index == _current_sprite->index) {
                change_occurred = true;
                part            = frame.parts.erase(part);
            } else part++;
        }

        // Recompute indices if necessary
        if (change_occurred) {
            ushort index = 0;
            for (auto& part : frame.parts)
                part.index = index++;

            // Update flag
            if (frame.index == _current_frame->index)
                current_frame_changed = true;
        }
    }

    // Remove Sprite
    _opd->sprites.erase(_current_sprite);

    // Update indices
    ushort index = 0;
    for (auto& sprite : _opd->sprites)
        sprite.index = index++;

    // === UPDATE UI ===
    // Get current lwi
    const auto sprite_lwi =
        dynamic_cast<SpriteLwi*>(ui->list_sprites->currentItem());
    if (sprite_lwi == nullptr) return;

    // Update animation tab if necessary
    if (current_frame_changed) {
        // Find current frame part list
        const auto frame_part_row = ui->list_frame_parts->currentRow();
        if (frame_part_row != -1)
            // Remove it
            delete ui->list_frame_parts->takeItem(frame_part_row);
    }

    // Remove sprite itself
    ui->list_sprites->removeItemWidget(sprite_lwi);
    delete sprite_lwi;

    // If this is the last sprite
    if (_opd->sprites.size() == 0) clear_sprite();
}

void MainWindow::on_bt_sprite_trim_clicked() {
    check_if_valid(_current_sprite);

    if (_current_sprite_import != nullptr) {
        _current_sprite_import->trim();

        change_ui(spin_sprite_pos_x, setValue(_current_sprite_import->x_pos));
        change_ui(spin_sprite_pos_y, setValue(_current_sprite_import->y_pos));
        change_ui(spin_sprite_width, setValue(_current_sprite_import->width));
        change_ui(spin_sprite_height, setValue(_current_sprite_import->height));
    } else {
        // === UPDATE VALUE ===
        save_previous_state();
        _current_sprite->trim();

        // === UPDATE UI ===
        change_ui(spin_sprite_pos_x, setValue(_current_sprite->x_pos));
        change_ui(spin_sprite_pos_y, setValue(_current_sprite->y_pos));
        change_ui(spin_sprite_width, setValue(_current_sprite->width));
        change_ui(spin_sprite_height, setValue(_current_sprite->height));
    }
    redraw_sprite();
}

void MainWindow::on_bt_merge_layers_clicked() {
    check_if_valid(_current_sprite);
    if (_current_sprite_import == nullptr) return;

    // === UPDATE VALUE ===
    save_previous_state();
    // Replace sprite
    _current_sprite_import->index = _current_sprite->index;
    _current_sprite_import->uses  = _current_sprite->uses;
    *_current_sprite              = *_current_sprite_import;

    // Replace palette
    update_palettes(
        _current_sprite_palette.index, _current_sprite_import_palette
    );

    // Clear import
    delete _current_sprite_import;
    _current_sprite_import = nullptr;
}
void MainWindow::on_bt_save_sprite_clicked() {
    check_if_valid(_current_sprite);
    if (_current_sprite->width == 0 || _current_sprite->height == 0) return;

    // Compute pixel map
    const auto image = _current_sprite->to_image(_current_sprite_palette);

    // Save image
    save_PNG(image);
}
void MainWindow::on_ch_background_enabled_toggled(bool new_value) {
    check_if_valid(_current_sprite);

    ui->gv_sprite->with_background = new_value;
    redraw_sprite();
}
void MainWindow::on_slider_transparency_valueChanged(int new_value) {
    check_if_valid(_current_sprite);
    redraw_sprite();
}

void MainWindow::on_spin_sprite_pos_x_valueChanged(int new_value) {
    check_if_valid(_current_sprite);

    if (!_current_sprite_import) {
        // === UPDATE VALUE ===
        save_previous_state();
        _current_sprite->x_pos = new_value;
    } else _current_sprite_import->x_pos = new_value;

    redraw_sprite();
}
void MainWindow::on_spin_sprite_pos_y_valueChanged(int new_value) {
    check_if_valid(_current_sprite);

    if (!_current_sprite_import) {
        // === UPDATE VALUE ===
        save_previous_state();
        _current_sprite->y_pos = new_value;
    } else _current_sprite_import->y_pos = new_value;

    redraw_sprite();
}
void MainWindow::on_spin_sprite_width_valueChanged(int new_value) {
    check_if_valid(_current_sprite);

    if (!_current_sprite_import) {
        // === UPDATE VALUE ===
        save_previous_state();
        _current_sprite->width = new_value;
    } else _current_sprite_import->width = new_value;

    redraw_sprite();
}
void MainWindow::on_spin_sprite_height_valueChanged(int new_value) {
    check_if_valid(_current_sprite);

    if (!_current_sprite_import) {
        // === UPDATE VALUE ===
        save_previous_state();
        _current_sprite->height = new_value;
    } else _current_sprite_import->height = new_value;

    redraw_sprite();
}

void MainWindow::on_cb_sprite_palette_currentIndexChanged(int new_index) {
    if (new_index < 0) return;
    const auto cb = ui->cb_sprite_palette;

    // If "Add New" option clicked
    if (new_index != 0 && new_index == cb->count() - 1) {
        // Update combo box
        cb->setItemText(new_index, QString::number(new_index));

        // And all other palette combo boxes
        ui->cb_frame_part_color_set->addItem(QString::number(new_index));

        // New palette unlocked
        _opd->palette_count++;

        // Add "Add New" option if possible
        if (_opd->palette_count < _opd->palette_max) cb->addItem("Add New");
    }

    // Set current palette
    _current_sprite_palette = _opd->palettes[new_index];
    bt_sprite_col_ALL(set_color());

    redraw_sprite();
}

// -----------------------------------------------------------------------------
// Sprite palette buttons
// -----------------------------------------------------------------------------

#define button_click_method(i, j)                                              \
    void MainWindow::on_bt_sprite_col_##i##_clicked() {                        \
        on_bt_sprite_col_clicked(ui->bt_sprite_col_##i);                       \
    }

APPLY_TO_COL(button_click_method);

// ////////////////////////////////// //
// MAIN WINDOW SPRITE PRIVATE METHODS //
// ////////////////////////////////// //

void MainWindow::load_sprites() {
    // Setup palettes
    ui->cb_sprite_palette->clear();
    change_ui(cb_sprite_palette, addItem("Default"));
    change_ui(cb_sprite_palette, addItem("Add New"));
    for (auto i = 1; i < _opd->palette_count; i++) {
        change_ui(cb_sprite_palette, insertItem(i, QString::number(i)));
    }
    on_cb_sprite_palette_currentIndexChanged(0); // Initialize Default
    _current_sprite_palette = *_opd->palettes.begin();

    // Add all loaded sprites
    ui->list_sprites->clear();
    ForEach(sprite, _opd->sprites)
        ui->list_sprites->addItem(new SpriteLwi(sprite));

    // Select first
    ui->list_sprites->setCurrentRow(0);
}
void MainWindow::load_sprite(const SpritePtr sprite) {
    _current_sprite = sprite;

    // Clear import sprite
    if (_current_sprite_import) {
        delete _current_sprite_import;
        _current_sprite_import = nullptr;
    }

    // Update ui
    change_ui(spin_sprite_pos_x, setValue(sprite->x_pos));
    change_ui(spin_sprite_pos_y, setValue(sprite->y_pos));
    change_ui(spin_sprite_width, setValue(sprite->width));
    change_ui(spin_sprite_height, setValue(sprite->height));
    redraw_sprite();

    // Enable editing
    set_sprite_edit_enabled(true);
}
void MainWindow::clear_sprite() {
    _current_sprite = Invalid::sprite;

    // Clear import sprite
    if (_current_sprite_import) {
        delete _current_sprite_import;
        _current_sprite_import = nullptr;
    }

    // Update ui
    ui->gv_sprite->clear();
    change_ui(spin_sprite_pos_x, clear());
    change_ui(spin_sprite_pos_y, clear());
    change_ui(spin_sprite_width, clear());
    change_ui(spin_sprite_height, clear());

    // Disable editing
    set_sprite_edit_enabled(false);
}

void MainWindow::on_bt_sprite_col_clicked(PaletteButton* const button) {
    if (button->color_index >= _current_sprite_palette.size) return;

    // Compute new color
    auto color = _current_sprite_palette[button->color_index];
    prompt_color_dialog(color);

    // Update palettes
    update_palettes(_current_sprite_palette.index, button->color_index, color);
}

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

void MainWindow::redraw_sprite() {
    if (_redrawing_sprite) return;
    ui->gv_sprite->show_sprite(*_current_sprite, _current_sprite_palette);
    if (_current_sprite_import)
        ui->gv_sprite->add_sprite(
            *_current_sprite_import,
            _current_sprite_import_palette,
            1.0f - ui->slider_transparency->value() / 100.0f
        );
}