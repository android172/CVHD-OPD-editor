#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "gui/lwi/image_lwi.h"

#include <QFileDialog>

#define redraw(pixels)                                                         \
    ui->gv_image->show_image(pixels, _image_palette);                          \
    ui->gv_image->add_selection(                                               \
        ui->spin_img_pos_x->value(),                                           \
        ui->spin_img_pos_y->value(),                                           \
        ui->spin_img_width->value(),                                           \
        ui->spin_img_height->value()                                           \
    )

QString all_supported_img_formats() {
    QString ret = "Images (";

    for (const auto& format : MainWindow::supported_image_formats) {
        ret += QString("*.") + format + " ";
    }
    ret.chop(2);
    ret += ")";
    return ret;
}

// /////////////////////// //
// MAIN WINDOW IMAGE SLOTS //
// /////////////////////// //

void MainWindow::on_list_images_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    const auto image_lwi = dynamic_cast<ImageLWI*>(current);
    if (image_lwi == nullptr) return;
    redraw(image_lwi->pixels());
}

void MainWindow::on_bt_import_image_clicked() {
    auto file_list = QFileDialog::getOpenFileNames(
        this,
        "Import images",
        _default_image_import_location,
        all_supported_img_formats()
    );
    load_images(file_list);
}
void MainWindow::on_bt_delete_image_clicked() {
    // Delete current
    const auto image_index = ui->list_images->currentIndex();
    if (!image_index.isValid()) return;
    ui->list_images->takeItem(image_index.row());

    // Update rest if last
    if (ui->list_images->count() == 0) {
        ui->gv_image->clear();
        set_image_edit_enabled(false);
    }
}
void MainWindow::on_bt_delete_all_images_clicked() {
    ui->list_images->clear();
    ui->gv_image->clear();
    set_image_edit_enabled(false);
}

void MainWindow::on_bt_select_rect_clicked() {
    ui->gv_image->activate_selection(
        [=](short x, short y, ushort width, ushort height) {
            ui->spin_img_pos_x->setValue(x);
            ui->spin_img_pos_y->setValue(y);
            ui->spin_img_width->setValue(width);
            ui->spin_img_height->setValue(height);
        }
    );
}
void MainWindow::on_bt_import_selected_clicked() {
    check_if_valid(_current_sprite);

    // Get selection info
    const auto x = ui->spin_img_pos_x->value();
    const auto y = ui->spin_img_pos_y->value();
    const auto w = ui->spin_img_width->value();
    const auto h = ui->spin_img_height->value();

    if (w == 0 || h == 0) return;

    // Get current image
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;

    // Import image as sprite
    import_image_as_sprite(image_lwi->pixels(), x, y, w, h);
}
void MainWindow::on_bt_import_all_clicked() {
    check_if_valid(_current_sprite);

    // Get current image
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;
    const auto pixels = image_lwi->pixels();

    // Get selection info
    const auto x = 0;
    const auto y = 0;
    const auto h = pixels.size();
    if (h == 0) return;
    const auto w = pixels[0].size();
    if (w == 0) return;

    // Import image as sprite
    import_image_as_sprite(pixels, x, y, w, h);
}

void MainWindow::on_bt_import_palette_clicked() {}

void MainWindow::on_bt_image_trim_clicked() {
    check_if_valid(_current_sprite);

    // Get current image
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;
    const auto& pixels = image_lwi->pixels();

    // Get image info
    const auto img_h = pixels.size();
    if (img_h == 0) return;
    const auto img_w = pixels[0].size();
    if (img_w == 0) return;

    // Get selection info
    auto x = ui->spin_img_pos_x->value();
    auto y = ui->spin_img_pos_y->value();
    auto w = ui->spin_img_width->value();
    auto h = ui->spin_img_height->value();
    if (w == 0 || h == 0) return;

    // Create temp pixmap
    PixelMap selection {};
    selection.resize(h);
    for (auto i = 0; i < h; i++) {
        selection[i].resize(w);
        const auto py = y + i;
        for (auto j = 0; j < w; j++) {
            const auto px   = x + j;
            selection[i][j] = (py >= 0 && py < img_h && px >= 0 && px < img_w)
                                  ? pixels[y + i][x + j]
                                  : 0;
        }
    }

    // Trim empty rows
    for (auto i = 0; i < selection.size(); i++) {
        if (selection.row_empty(i)) {
            y++;
            h--;
        } else break;
    }
    if (h == 0) {
        // Update selection
        ui->spin_img_width->setValue(0);
        ui->spin_img_height->setValue(0);
        redraw(pixels);
    }
    for (auto i = selection.size() - 1; i >= 0; i--) {
        if (selection.row_empty(i)) h--;
        else break;
    }
    // Trim empty columns
    for (auto i = 0; i < selection[0].size(); i++) {
        if (selection.column_empty(i)) {
            x++;
            w--;
        } else break;
    }
    for (auto i = selection[0].size() - 1; i >= 0; i--) {
        if (selection.column_empty(i)) w--;
        else break;
    }

    // Update selection
    ui->spin_img_pos_x->setValue(x);
    ui->spin_img_pos_y->setValue(y);
    ui->spin_img_width->setValue(w);
    ui->spin_img_height->setValue(h);
    redraw(pixels);
}

void MainWindow::on_spin_img_pos_x_valueChanged(int new_value) {
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;
    redraw(image_lwi->pixels());
}
void MainWindow::on_spin_img_pos_y_valueChanged(int new_value) {
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;
    redraw(image_lwi->pixels());
}
void MainWindow::on_spin_img_width_valueChanged(int new_value) {
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;
    redraw(image_lwi->pixels());
}
void MainWindow::on_spin_img_height_valueChanged(int new_value) {
    const auto image_lwi =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (image_lwi == nullptr) return;
    redraw(image_lwi->pixels());
}

// -----------------------------------------------------------------------------
// Sprite palette buttons
// -----------------------------------------------------------------------------

#define button_click_method(i, j)                                              \
    void MainWindow::on_bt_image_col_##i##_clicked() {                         \
        on_bt_image_col_clicked(ui->bt_image_col_##i);                         \
    }

APPLY_TO_COL(button_click_method);

// ///////////////////////////////// //
// MAIN WINDOW IMAGE PRIVATE METHODS //
// ///////////////////////////////// //

void MainWindow::load_images(QStringList path_list) {
    // Image list
    auto list = ui->list_images;

    for (const auto& file_path : path_list) {
        // Load image
        auto image = QImage(file_path);

        // Initialize if necessary
        if (list->count() == 0) {
            set_image_edit_enabled(true);
            setup_image_palette(image);

            bt_image_col_ALL(set_color());
        } else if (_image_palette.size() < 16) {
            update_image_palette(image);

            // Inform images of changed palette
            for (int i = 0; i < ui->list_images->count(); i++) {
                auto image = dynamic_cast<ImageLWI*>(ui->list_images->item(i));
                image->update_color();
            }

            // Setup button colors
            bt_image_col_ALL(set_color());
        }

        // Save to image list
        const QString image_name = file_path.split('/').last();
        const auto image_item = new ImageLWI(image_name, image, &_color_index);
        list->addItem(image_item);
    }

    if (path_list.size() != 0) {
        // Remember last import location
        const QString file_name        = path_list[0].split('/').last();
        const QString file_dir         = path_list[0].chopped(file_name.size());
        _default_image_import_location = file_dir;
        save_app_state();

        list->setCurrentRow(0);
    }
}

void MainWindow::on_bt_image_col_clicked(ColorButton* const button) {}

void MainWindow::import_image_as_sprite(
    const PixelMap& pixels,
    const short     x,
    const short     y,
    const ushort    w,
    const ushort    h
) {
    // Add new sprite import
    if (_current_sprite_import != nullptr) delete _current_sprite_import;
    _current_sprite_import = new Sprite();
    _current_sprite_import->from_image(pixels, x, y, w, h);

    // Set it's palette
    _current_sprite_import_palette.size = _image_palette.size();
    for (auto i = 0; i < _image_palette.size(); i++)
        _current_sprite_import_palette[i] = _image_palette[i].display;

    // Add layer to sprite view
    ui->gv_sprite->add_sprite(
        *_current_sprite_import,
        _current_sprite_import_palette,
        1.0f - ui->slider_transparency->value() / 100.0f
    );

    // Update UI
    ui->spin_sprite_pos_x->setValue(_current_sprite_import->x_pos);
    ui->spin_sprite_pos_y->setValue(_current_sprite_import->y_pos);
    ui->spin_sprite_width->setValue(_current_sprite_import->width);
    ui->spin_sprite_height->setValue(_current_sprite_import->height);
}

void MainWindow::set_image_edit_enabled(bool enabled) {
    ui->bt_delete_image->setEnabled(enabled);
    ui->bt_delete_all_images->setEnabled(enabled);
    ui->bt_select_rect->setEnabled(enabled);
    ui->bt_import_selected->setEnabled(enabled);
    ui->bt_import_all->setEnabled(enabled);
    ui->bt_import_palette->setEnabled(enabled);
    ui->bt_image_trim->setEnabled(enabled);
    ui->spin_img_pos_x->setEnabled(enabled);
    ui->spin_img_pos_y->setEnabled(enabled);
    ui->spin_img_width->setEnabled(enabled);
    ui->spin_img_height->setEnabled(enabled);
    bt_image_col_ALL(setEnabled(enabled));
}
