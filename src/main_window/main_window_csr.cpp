#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "gui/lwi/csr_lwi.h"

// ///////////////////// //
// MAIN WINDOW CSR SLOTS //
// ///////////////////// //

void MainWindow::on_list_csrs_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    const auto csr_lwi = dynamic_cast<CSRLwi*>(current);
    if (csr_lwi == nullptr) return;
    load_csr(csr_lwi->csr);
}

void MainWindow::on_bt_export_csr_clicked() {
    check_if_valid(_current_csr);

    // Compute pixel map
    const auto image = _current_csr->to_image(_current_csr_palette);

    // Save PNG
    save_PNG(image);
}
void MainWindow::on_bt_save_col_clicked() {
    check_if_valid(_current_csr);

    // Update palettes
    update_palettes(_current_csr_palette.index, _current_csr_palette);
}
void MainWindow::on_cb_csr_palette_currentIndexChanged(int new_index) {
    if (new_index == -1) return;

    // Set current palette
    _current_csr_palette = _opd->palettes[new_index];
    bt_csr_col_ALL(set_color());
    redraw_csr();
}

// -----------------------------------------------------------------------------
// CSR palette buttons
// -----------------------------------------------------------------------------

#define button_click_method(i, j)                                              \
    void MainWindow::on_bt_csr_col_##i##_clicked() {                           \
        on_bt_csr_col_clicked(ui->bt_csr_col_##i);                             \
    }

APPLY_TO_COL(button_click_method);

// /////////////////////////////// //
// MAIN WINDOW CSR PRIVATE METHODS //
// /////////////////////////////// //

void MainWindow::load_csrs() {
    // Setup palettes
    ui->cb_csr_palette->clear();
    ui->cb_csr_palette->addItem("Default");
    for (auto i = 1; i < _opd->palette_count; i++)
        ui->cb_csr_palette->insertItem(i, QString::number(i));
    _current_csr_palette = *_opd->palettes.begin();

    // Add all loaded sprites
    ui->list_csrs->clear();
    ForEach(csr, _opd->gfx_pages) ui->list_csrs->addItem(new CSRLwi(csr));

    // Select first
    ui->list_csrs->setCurrentRow(0);
}
void MainWindow::load_csr(const GFXPagePtr csr) {
    _current_csr = csr;

    // Update ui
    redraw_csr();

    // Enable editing
    set_csr_edit_enabled(true);
}
void MainWindow::clear_csr() {
    _current_csr = Invalid::gfx_page;

    // Update ui
    ui->gv_csr->clear();

    // Disable editing
    set_csr_edit_enabled(false);
}

void MainWindow::on_bt_csr_col_clicked(PaletteButton* const button) {
    if (button->color_index >= _current_csr_palette.size) return;

    // Compute new color
    prompt_color_dialog(_current_csr_palette[button->color_index]);

    // Setup button color
    button->set_color();

    // Redraw frame
    redraw_csr();
}

void MainWindow::set_csr_edit_enabled(bool enabled) {
    ui->bt_export_csr->setEnabled(enabled);
    ui->bt_save_col->setEnabled(enabled);
    ui->cb_csr_palette->setEnabled(enabled);
    bt_csr_col_ALL(setEnabled(enabled));
}

void MainWindow::redraw_csr() {
    if (_redrawing_csr) return;
    ui->gv_csr->show_csr(*_current_csr, _current_csr_palette);
}
