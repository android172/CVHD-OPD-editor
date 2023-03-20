#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include <QFileDialog>
#include <QDesktopServices>

// ///////////////////// //
// MAIN WINDOW OPD SLOTS //
// ///////////////////// //

void MainWindow::on_bt_open_opd_clicked() {
    // Load OPD
    const auto opd_path = QFileDialog::getOpenFileName(
        this, "Import CSR file", _default_opd_import_location, "CHD_opd (*.opd)"
    );
    import_opd(opd_path);
}
void MainWindow::on_bt_save_opd_clicked() { export_opd(); }

void MainWindow::on_bt_open_folder_clicked() {
    // Get the parent directory of the file
    QString dir_path = QFileInfo(_opd->file_path).dir().path();

    // Open the directory in the file explorer
    QDesktopServices::openUrl(QUrl::fromLocalFile(dir_path));
}

// /////////////////////////////// //
// MAIN WINDOW OPD PRIVATE METHODS //
// /////////////////////////////// //

void MainWindow::import_opd(const QString opd_path) {
    if (opd_path.isEmpty()) return;
    _opd = Opd::open(opd_path);

    // Save default_location
    const auto opd_name          = opd_path.split('/').last();
    _default_opd_import_location = opd_path.chopped(opd_name.size());
    save_app_state();

    // Save to recent
    add_to_recent(opd_path);

    // Update palette count
    ui->cb_frame_part_color_set->clear();
    ui->cb_frame_part_color_set->addItem("Default");
    for (auto i = 1; i < _opd->palette_count; i++)
        ui->cb_frame_part_color_set->addItem(QString::number(i));

    // Load animations, sprites & CSRs
    load_animations();
    load_sprites();
    load_csrs();

    // Enable editing
    set_opd_edit_enabled(true);

    // Set name
    ui->label_opd_name->setText(_opd->file_name);
}

void MainWindow::export_opd() {
    // Save OPD
    _opd->save();

    // Update CSR page
    _current_csr = Invalid::gfx_page;

    // Reload
    load_csrs();
    on_tree_animations_itemPressed(ui->tree_animations->currentItem(), 0);
}

void MainWindow::set_opd_edit_enabled(bool enabled) {
    ui->tab_main->setTabEnabled(1, enabled);
    ui->tab_main->setTabEnabled(2, enabled);
    ui->bt_add_frame->setEnabled(enabled);
    ui->bt_open_folder->setEnabled(enabled);
    ui->bt_save_opd->setEnabled(enabled);
    set_menu_bar_edit_enabled(enabled);
}