#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include <QFileDialog>

// ////////////////////////// //
// MAIN WINDOW MENU BAR SLOTS //
// ////////////////////////// //

void MainWindow::on_action_open_opd_triggered() {
    // Load OPD
    const auto opd_path = QFileDialog::getOpenFileName(
        this, "Import CSR file", _default_opd_import_location, "CHD_opd (*.opd)"
    );
    import_opd(opd_path);
}

void MainWindow::on_action_save_opd_triggered() { export_opd(); }

void MainWindow::on_action_exit_triggered() {
    exit(EXIT_SUCCESS); // :D
}

void MainWindow::on_action_change_value_globally_triggered() {}

// //////////////////////////////////// //
// MAIN WINDOW MENU BAR PRIVATE METHODS //
// //////////////////////////////////// //

void MainWindow::add_to_recent(const QString new_opd_path) {
    // Check if change is necessary at all
    if (_recently_opd_paths.size() &&
        _recently_opd_paths[0].compare(new_opd_path) == 0)
        return;

    // Take from the list if already exists
    ForEach(opd_path, _recently_opd_paths) {
        if (opd_path->compare(new_opd_path) == 0) {
            _recently_opd_paths.erase(opd_path);
            break;
        }
    }

    // Add opd path to recent list
    _recently_opd_paths.push_front(new_opd_path);

    // Drop all paths past the first 10
    while (_recently_opd_paths.size() > 10)
        _recently_opd_paths.pop_back();

    // Refresh menubar
    update_recent_menubar();
}

void MainWindow::update_recent_menubar() {
    if (_recently_opd_paths.size() == 0) return;
    ui->menu_recent->clear();
    for (const auto& opd_path : _recently_opd_paths) {
        const auto recent_path_action = new QAction(opd_path, this);

        // Elided text (if req)
        QFontMetrics font_metrics(recent_path_action->font());
        if (font_metrics.width(recent_path_action->text()) > 500) {
            const auto text = font_metrics.elidedText(
                recent_path_action->text(), Qt::ElideLeft, 500
            );
            recent_path_action->setText(text);
        }

        // Connect on click
        connect(recent_path_action, &QAction::triggered, this, [=](bool) {
            import_opd(opd_path);
        });

        // Add to menubar
        ui->menu_recent->addAction(recent_path_action);
    }
}

void MainWindow::set_menu_bar_edit_enabled(bool enabled) {
    ui->action_save_opd->setEnabled(enabled);
    ui->action_save_opd->setEnabled(enabled);
    ui->action_change_value_globally->setEnabled(enabled);
}