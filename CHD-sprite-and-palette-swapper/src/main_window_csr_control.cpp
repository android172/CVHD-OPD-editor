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
    if (csrs->count() == 0) {
        set_csr_section_enabled(false);
        _current_csr_path = "";
    }
}
void MainWindow::on_bt_delete_all_csr_clicked() {
    // Delete all
    ui->list_csrs->clear();

    // Update other elements
    set_csr_section_enabled(false);
    _current_csr_path = "";
}
void MainWindow::on_list_csrs_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    auto csr = dynamic_cast<CsrLWI*>(current);
    if (!csr) return;
    _current_csr_path = csr->path;
}

#include <iostream>

void MainWindow::on_bt_image_to_csr_clicked() {
    if (_current_csr_path.isEmpty()) return;

    // Get current image
    auto current_image =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());
    if (!current_image) return;

    // Compute current image hex
    QString image_hex {};
    bool    even = false;
    char    temp;
    for (const auto& row : current_image->pixels()) {
        for (const auto& pixel : row) {
            // std::cout << (int) pixel << " ";
            if (even) {
                image_hex += temp | pixel;
            } else temp = pixel << 4;
            even = !even;
        }
        // std::cout << std::endl;
    }

    // Open appropriate csr file
    std::ofstream csr_file { _current_csr_path.toStdString(),
                             std::ios::binary | std::ios::out | std::ios::in };
    if (!csr_file.is_open()) return;

    csr_file.seekp(0x230);
    csr_file.write(image_hex.toStdString().data(), image_hex.size());

    csr_file.close(); // x2B0
}

// /////////////////////////////////////// //
// MAIN WINDOW CSR CONTROL PRIVATE METHODS //
// /////////////////////////////////////// //

void MainWindow::import_csrs(QStringList path_list) {
    // CSR list
    auto csrs = ui->list_csrs;

    for (auto& file_path : path_list) {
        // Get file name
        const QString file_name = file_path.split('/').last();
        auto          csr_item  = new CsrLWI(file_name, file_path);
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
    ui->bt_image_to_csr->setEnabled(is_enabled && !_image_list_empty);
}