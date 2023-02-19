#include "main_window.h"
#include "../forms/ui_main_window.h"

#include "image_lwi.h"

#include <fstream>
#include <QColorDialog>
#include <QFileDialog>

// ///////////////////////// //
// MAIN WINDOW PALETTE SLOTS //
// ///////////////////////// //

#define on_click(fun, r_fun, butt)                                             \
    void MainWindow::fun() { on_img_palette_button_clicked(ui->butt); }        \
    void MainWindow::r_fun() { on_img_palette_button_right_clicked(ui->butt); }

on_click(on_bt_col_0_clicked, on_bt_col_0_right_clicked, bt_col_0);
on_click(on_bt_col_1_clicked, on_bt_col_1_right_clicked, bt_col_1);
on_click(on_bt_col_2_clicked, on_bt_col_2_right_clicked, bt_col_2);
on_click(on_bt_col_3_clicked, on_bt_col_3_right_clicked, bt_col_3);
on_click(on_bt_col_4_clicked, on_bt_col_4_right_clicked, bt_col_4);
on_click(on_bt_col_5_clicked, on_bt_col_5_right_clicked, bt_col_5);
on_click(on_bt_col_6_clicked, on_bt_col_6_right_clicked, bt_col_6);
on_click(on_bt_col_7_clicked, on_bt_col_7_right_clicked, bt_col_7);
on_click(on_bt_col_8_clicked, on_bt_col_8_right_clicked, bt_col_8);
on_click(on_bt_col_9_clicked, on_bt_col_9_right_clicked, bt_col_9);
on_click(on_bt_col_A_clicked, on_bt_col_A_right_clicked, bt_col_A);
on_click(on_bt_col_B_clicked, on_bt_col_B_right_clicked, bt_col_B);
on_click(on_bt_col_C_clicked, on_bt_col_C_right_clicked, bt_col_C);
on_click(on_bt_col_D_clicked, on_bt_col_D_right_clicked, bt_col_D);
on_click(on_bt_col_E_clicked, on_bt_col_E_right_clicked, bt_col_E);
on_click(on_bt_col_F_clicked, on_bt_col_F_right_clicked, bt_col_F);

void MainWindow::on_cb_color_set_textActivated(QString color_set) {
    // Update current csr pallet
    _current_col_set = 0;
    if (color_set.compare("Default") != 0) _current_col_set = color_set.toInt();
    _csr_palette = _csr_palettes[_current_col_set];

    // Update button colors
    bt_icol_ALL(set_color());

    // Update presented image
    if (_csr_image_presented)
        on_list_csrs_itemPressed(ui->list_csrs->currentItem());
}

// ///////////////////// //
// MAIN WINDOW COL SLOTS //
// ///////////////////// //

#define on_col_click(fun, butt)                                                \
    void MainWindow::fun() { on_csr_palette_button_clicked(ui->butt); }

on_col_click(on_bt_icol_0_clicked, bt_icol_0);
on_col_click(on_bt_icol_1_clicked, bt_icol_1);
on_col_click(on_bt_icol_2_clicked, bt_icol_2);
on_col_click(on_bt_icol_3_clicked, bt_icol_3);
on_col_click(on_bt_icol_4_clicked, bt_icol_4);
on_col_click(on_bt_icol_5_clicked, bt_icol_5);
on_col_click(on_bt_icol_6_clicked, bt_icol_6);
on_col_click(on_bt_icol_7_clicked, bt_icol_7);
on_col_click(on_bt_icol_8_clicked, bt_icol_8);
on_col_click(on_bt_icol_9_clicked, bt_icol_9);
on_col_click(on_bt_icol_A_clicked, bt_icol_A);
on_col_click(on_bt_icol_B_clicked, bt_icol_B);
on_col_click(on_bt_icol_C_clicked, bt_icol_C);
on_col_click(on_bt_icol_D_clicked, bt_icol_D);
on_col_click(on_bt_icol_E_clicked, bt_icol_E);
on_col_click(on_bt_icol_F_clicked, bt_icol_F);

void MainWindow::on_bt_import_col_clicked() {
    const auto file_path = QFileDialog::getOpenFileName(
        this, "Import col file", _default_col_import_location, "CHD_col (*.col)"
    );
    if (!file_path.isEmpty()) import_col(file_path);
}

void MainWindow::on_bt_palette_to_col_clicked() {
    // Transfer colors to csr palette
    for (int i = 0; i < _image_palette.size(); i++) {
        const Color& color                 = _image_palette[i].display;
        _csr_palettes[_current_col_set][i] = { color, color };
    }

    // Inform others of this change
    on_cb_color_set_textActivated(ui->cb_color_set->currentText());
}

void MainWindow::on_bt_save_col_clicked() {
    // Compute current palette hex
    std::string palette_hex {};
    for (const auto& color_pair : _csr_palette) {
        Color c = color_pair.display;
        palette_hex += c.b / _current_col_multiplier;
        palette_hex += c.g / _current_col_multiplier;
        palette_hex += c.r / _current_col_multiplier;
        palette_hex += '\000';
    }

    // Open col file
    std::ofstream col_file { _current_col_path.toStdString(),
                             std::ios::out | std::ios::binary | std::ios::in };
    col_file.seekp(0x20 + _current_col_set * 0x40);

    // Replace color bytes
    col_file.write(palette_hex.data(), palette_hex.size());
    col_file.close();
}

// /////////////////////////////////////////// //
// MAIN WINDOW PALETTE CONTROL PRIVATE METHODS //
// /////////////////////////////////////////// //

void MainWindow::on_img_palette_button_clicked(ColorButton* button) {
    if (_image_palette.size() <= button->color_index) return;

    auto   c = _image_palette[button->color_index].display;
    QColor initial_color { c.r, c.g, c.b };

    QColor new_color = QColorDialog::getColor(initial_color);
    if (new_color.isValid() == false) return;
    Color nc { (uchar) new_color.red(),
               (uchar) new_color.green(),
               (uchar) new_color.blue() };
    _image_palette[button->color_index].display = nc;

    // Setup button colors
    button->set_color();

    // Reload drawn image
    if (_csr_image_presented == false)
        on_list_images_itemPressed(ui->list_images->currentItem());
}

void MainWindow::on_img_palette_button_right_clicked(ColorButton* button) {
    if (button->color_index == 0 ||
        _image_palette.size() <= button->color_index)
        return;

    // Rearrange palette
    const auto temp                     = _image_palette[button->color_index];
    _image_palette[button->color_index] = _image_palette[0];
    _image_palette[0]                   = temp;

    // Sort & recompute hash
    std::sort(_image_palette.begin() + 1, _image_palette.end());
    _color_index.clear();
    for (int i = 0; i < _image_palette.size(); ++i)
        _color_index[_image_palette[i].original] = i;

    // Inform images of changed palette
    for (int i = 0; i < ui->list_images->count(); i++) {
        auto image = dynamic_cast<ImageLWI*>(ui->list_images->item(i));
        image->update_color();
    }

    // Setup button colors
    bt_col_ALL(set_color());
}

void MainWindow::setup_image_palette(QImage image) {
    // Compute palette
    _image_palette.clear();
    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            QColor pixel { image.pixel(j, i) };
            Color  color { (uchar) pixel.red(),
                          (uchar) pixel.green(),
                          (uchar) pixel.blue() };

            if (_image_palette.contains({ color, color }) == false)
                _image_palette.push_back({ color, color });
        }
    }
    if (_image_palette.size() > 16) _image_palette.resize(16);
    std::sort(_image_palette.begin() + 1, _image_palette.end());
    // Compute util palette structures
    _color_index.clear();
    for (int i = 0; i < _image_palette.size(); ++i)
        _color_index[_image_palette[i].original] = i;

    // Setup button colors
    bt_col_ALL(set_color());
}

void MainWindow::update_image_palette(QImage image) {
    // Compute palette
    auto initial_size = _image_palette.size();
    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            QColor pixel { image.pixel(j, i) };
            Color  color { (uchar) pixel.red(),
                          (uchar) pixel.green(),
                          (uchar) pixel.blue() };

            if (_image_palette.contains({ color, color }) == false)
                _image_palette.push_back({ color, color });
        }
    }
    if (_image_palette.size() > 16) _image_palette.resize(16);
    if (initial_size < _image_palette.size()) {
        std::sort(_image_palette.begin() + 1, _image_palette.end());
        _color_index.clear();
        for (int i = 0; i < _image_palette.size(); ++i)
            _color_index[_image_palette[i].original] = i;
    }

    // Inform images of changed palette
    for (int i = 0; i < ui->list_images->count(); i++) {
        auto image = dynamic_cast<ImageLWI*>(ui->list_images->item(i));
        image->update_color();
    }

    // Setup button colors
    bt_col_ALL(set_color());
}

// -----------------------------------------------------------------------------
// COL
// -----------------------------------------------------------------------------

void MainWindow::on_csr_palette_button_clicked(ColorButton* button) {
    if (_csr_palette.size() <= button->color_index) return;

    // Get new color
    auto   c = _csr_palette[button->color_index].display;
    QColor initial_color { c.r, c.g, c.b };

    QColor new_color = QColorDialog::getColor(initial_color);
    if (new_color.isValid() == false) return;
    Color nc { (uchar) new_color.red(),
               (uchar) new_color.green(),
               (uchar) new_color.blue() };

    // Set new color
    auto& color_pair    = _csr_palettes[_current_col_set][button->color_index];
    color_pair.original = nc;
    color_pair.display  = nc;
    _csr_palette[button->color_index] = color_pair;

    // Setup button colors
    button->set_color();

    // Reload drawn image
    if (_csr_image_presented)
        on_list_csrs_itemPressed(ui->list_csrs->currentItem());
}

void MainWindow::import_col(QString file_path) {
    // === Process file info ===
    // Save location
    const QString file_name      = file_path.split('/').last();
    const QString file_dir       = file_path.chopped(file_name.size());
    _default_col_import_location = file_dir;
    save_app_state();

    // Set name and col path
    ui->label_col->setText(file_name);
    _current_col_path = file_path;

    // === Load data ===
    // Load palettes
    std::ifstream col_file { file_path.toStdString(),
                             std::ios::in | std::ios::binary };
    if (!col_file.is_open()) return;

    // Compute multiplier
    col_file.seekg(0x1F);
    col_file.read((char*) &_current_col_multiplier, 1);
    _current_col_multiplier =
        (_current_col_multiplier) ? _current_col_multiplier * 4 : 1;

    // Compute palettes (all 8)
    for (auto& palette : _csr_palettes) {
        palette.clear();
        // 16 colors
        for (int i = 0; i < 16; i++) {
            // 3 bytes each
            const int size = 3;
            char      buffer[size];

            // read palette color
            col_file.read(buffer, size);
            Color color = {
                (uchar) ((uchar) buffer[2] * _current_col_multiplier),
                (uchar) ((uchar) buffer[1] * _current_col_multiplier),
                (uchar) ((uchar) buffer[0] * _current_col_multiplier)
            };
            palette.push_back({ color, color });

            // read separation byte
            col_file.read(buffer, 1);
        }
    }

    col_file.close();

    // === Inform other components ===
    on_cb_color_set_textActivated(ui->cb_color_set->currentText());

    // Buttons enabled
    bt_icol_ALL(setEnabled(true));
    bt_icol_ALL(set_color());
    ui->bt_save_col->setEnabled(true);
    ui->cb_color_set->setEnabled(true);

    // Conditionally enabled
    if (!_image_list_empty) ui->bt_palette_to_col->setEnabled(true);
    if (!_csr_list_empty) ui->bt_export_csr->setEnabled(true);
}

// // Clipboard copy (for reference)
// QClipboard* clipboard = QGuiApplication::clipboard();
// clipboard->setText(palette_hex);
