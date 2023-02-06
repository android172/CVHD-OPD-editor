#include "main_window.h"
#include "ui_main_window.h"

#include "csr_lwi.h"
#include "image_lwi.h"

#include <QFileDialog>
#include <fstream>

// ///////////////////// //
// MAIN WINDOW CSR SLOTS //
// ///////////////////// //
void MainWindow::on_bt_import_csr_clicked() {
    auto file_list = QFileDialog::getOpenFileNames(
        this, "Import CSR file", _default_csr_import_location, "CHD_csr (*.csr)"
    );
    import_csrs(file_list);
}
void MainWindow::on_bt_delete_sel_csr_clicked() {
    auto csrs = ui->list_csrs;

    // Delete current
    auto current_csr = csrs->currentIndex();
    if (!current_csr.isValid()) return;
    csrs->takeItem(current_csr.row());

    // Update rest if last
    if (csrs->count() == 0) set_csr_section_enabled(false);
}
void MainWindow::on_bt_delete_all_csr_clicked() {
    // Delete all
    ui->list_csrs->clear();

    // Update other elements
    set_csr_section_enabled(false);
}

void MainWindow::on_list_csrs_itemPressed(QListWidgetItem* current) {
    auto csr = dynamic_cast<CsrLWI*>(current);
    if (!csr) return;

    _csr_image_presented = true;

    // Present csr image
    auto    image         = compute_pixel_map(csr->pixels(), _csr_palette);
    QPixmap present_image = QPixmap::fromImage(image);
    present_image         = present_image.scaled(
        512, 512, Qt::KeepAspectRatio, Qt::FastTransformation
    );
    ui->label_image_view->setPixmap(present_image);
}
void MainWindow::on_list_csrs_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    on_list_csrs_itemPressed(current);
}

void MainWindow::on_bt_image_to_csr_clicked() {
    // Get current image
    auto current_image =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (!current_image) return;

    // Get current csr
    auto current_csr = dynamic_cast<CsrLWI*>(ui->list_csrs->currentItem());
    if (!current_csr) return;

    // Compute current image hex
    std::string image_hex {};
    for (const auto& row : current_image->pixels()) {
        uint  row_idex = 0;
        uchar left_pixel;
        for (const auto& pixel : row) {
            if (row_idex % 2 == 1) image_hex += left_pixel | pixel;
            else left_pixel = pixel << 4;
            row_idex++;
        }
        if (row_idex < 128)
            for (uint i = row_idex / 2; i < 64; i++)
                image_hex += '\000';
    }

    // Open appropriate csr file
    std::ofstream csr_file { current_csr->path.toStdString(),
                             std::ios::binary | std::ios::out | std::ios::in };
    if (!csr_file.is_open()) return;

    // Write image data
    csr_file.seekp(0x230);
    csr_file.write(image_hex.data(), image_hex.size());
    csr_file.close();

    // Save to csr item
    for (int i = 0; i < ui->list_csrs->count(); i++) {
        auto csr_item = dynamic_cast<CsrLWI*>(ui->list_csrs->item(i));
        if (!csr_item) continue;
        if (csr_item->path.compare(current_csr->path) == 0)
            csr_item->import_data();
    }
    on_list_csrs_itemPressed(current_csr);
}

void MainWindow::on_bt_export_csr_clicked() {
    // Get current CSR
    const auto current_csr =
        dynamic_cast<CsrLWI*>(ui->list_csrs->currentItem());
    if (!current_csr) return;

    // Get save location
    auto path = QFileDialog::getSaveFileName(
        this, "Export csr as", _default_image_import_location, "PNG (*.png)"
    );
    if (path.isEmpty()) return;

    // Check extension
    const auto file_name = path.split('/').last();
    const auto file_dir  = path.chopped(file_name.size());
    const auto extension = file_name.split('.').last();
    if (extension.toLower().compare("png") != 0) path += ".png";

    // Compute pixel map
    const auto image = compute_pixel_map(current_csr->pixels(), _csr_palette);

    // And save it
    const auto result = image.save(path);
    if (!result) return;

    // Save this path
    _default_image_import_location = file_dir;
    save_app_state();
}

// /////////////////////////////////////// //
// MAIN WINDOW CSR CONTROL PRIVATE METHODS //
// /////////////////////////////////////// //

void MainWindow::import_csrs(QStringList path_list) {
    // CSR list
    auto csrs = ui->list_csrs;

    for (const auto& file_path : path_list) {
        // Get file name
        const QString file_name = file_path.split('/').last();
        const auto    csr_item  = new CsrLWI(file_name, file_path);
        csrs->addItem(csr_item);
    }

    if (path_list.size() != 0) {
        // Remember last import location
        const QString file_name      = path_list[0].split('/').last();
        const QString file_dir       = path_list[0].chopped(file_name.size());
        _default_csr_import_location = file_dir;
        save_app_state();

        if (_csr_list_empty) set_csr_section_enabled(true);
        csrs->setCurrentRow(0);
    }
}
void MainWindow::set_csr_section_enabled(bool is_enabled) {
    _csr_list_empty = !is_enabled;

    // Can be true only if image list is not empty
    ui->bt_image_to_csr->setEnabled(is_enabled && !_image_list_empty);

    // Toggles only if col path is present
    if (_current_col_path.size() > 0) ui->bt_export_csr->setEnabled(is_enabled);

    // On disable
    if (!is_enabled) {
        // Clear presented image if no imported image is present
        if (ui->list_images->count() > 0)
            on_list_images_itemPressed(ui->list_images->currentItem());
        else ui->label_image_view->clear();

        // In either case csr image isn't presented anymore
        _csr_image_presented = false;
    }
}
