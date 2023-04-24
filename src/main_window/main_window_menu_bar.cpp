#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "gui/global_change_dialog.h"

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

void MainWindow::on_action_undo_triggered() { undo(); }
void MainWindow::on_action_redo_triggered() { redo(); }

void MainWindow::on_action_change_value_globally_triggered() {
    const auto tree = ui->tree_animations;

    // Get all animation twi's
    QVector<QTreeWidgetItem*> animation_list {};
    animation_list.reserve(tree->topLevelItemCount());

    // Get all the animations (+ unused section optionally)
    for (auto i = 0; i < tree->topLevelItemCount(); i++)
        animation_list.push_back(tree->topLevelItem(i));

    // Create a global change dialog
    GlobalChangeDialog dialog { _opd };
    dialog.setModal(true);
    save_previous_state();
    if (dialog.exec()) on_tree_animations_itemPressed(tree->currentItem(), 0);
}

void MainWindow::on_action_flip_x_triggered() {}
void MainWindow::on_action_flip_y_triggered() {}
void MainWindow::on_action_trim_all_sprites_triggered() {}

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
        if (font_metrics.horizontalAdvance(recent_path_action->text()) > 500) {
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
    ui->action_undo->setEnabled(enabled);
    ui->action_redo->setEnabled(enabled);
    ui->action_change_value_globally->setEnabled(enabled);
    ui->action_flip_x->setEnabled(enabled);
    ui->action_flip_y->setEnabled(enabled);
    ui->action_trim_all_sprites->setEnabled(enabled);
}
