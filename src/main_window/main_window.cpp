#include "main_window.h"
#include "../../forms/ui_main_window.h"

#include "gui/twi/frame_twi.h"
#include "gui/twi/animation_twi.h"

#include <QFileDialog>
#include <QColorDialog>
#include <QMimeData>
#include <QScrollBar>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    qApp->installEventFilter(this);
    setWindowTitle("CVHD OPD editor");
    setAcceptDrops(true);
    load_app_state();

    // Initially disable editing
    set_general_edit_enabled(false);

    // Add default with background setting for GV gui items
    ui->gv_image->with_background  = true;
    ui->gv_sprite->with_background = true;
    ui->gv_csr->with_background    = true;

    // TODO: Temporary
    ui->bt_add_animation->setEnabled(false);
    ui->bt_remove_animation->setEnabled(false);

    // Setup color buttons
    setup_color_buttons();
}

MainWindow::~MainWindow() { delete ui; }

// ///////////////// //
// MAIN WINDOW SLOTS //
// ///////////////// //

bool MainWindow::eventFilter(QObject* obj, QEvent* event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent* key_event = static_cast<QKeyEvent*>(event);
        if (key_event->modifiers() == Qt::ControlModifier) {
            switch (key_event->key()) {
            case Qt::Key_Z: undo(); return true;
            case Qt::Key_Y: redo(); return true;
            default: break;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

// -----------------------------------------------------------------------------
// Drag and drop
// -----------------------------------------------------------------------------

void MainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mime_data { event->mimeData() };

    if (mime_data->hasUrls() == false) {
        event->ignore();
        return;
    }

    // Filter urls
    QStringList image_paths;
    QString     opd_path = "";
    for (int i = 0; i < mime_data->urls().size(); ++i) {
        const QUrl url = mime_data->urls().at(i);
        if (url.toString().startsWith("file:///")) {
            const QString path      = url.toLocalFile();
            const QString extension = path.split('.').last();
            if (supported_image_formats.contains(extension))
                image_paths.append(path);
            else if (extension.compare("opd") == 0) opd_path = path;
        }
    }

    // Import files
    if (image_paths.size() > 0) load_images(image_paths);
    if (opd_path.size() > 0) import_opd(opd_path);

    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    event->acceptProposedAction();
}
void MainWindow::dragMoveEvent(QDragMoveEvent* event) {
    event->acceptProposedAction();
}
void MainWindow::dragLeaveEvent(QDragLeaveEvent* event) { event->accept(); }

// /////////////////////////// //
// MAIN WINDOW PRIVATE METHODS //
// /////////////////////////// //

void MainWindow::load_ui() {
    // Update palette count
    ui->cb_frame_part_color_set->clear();
    change_ui(cb_frame_part_color_set, addItem("Default"));
    for (auto i = 1; i < _opd->palette_count; i++) {
        change_ui(cb_frame_part_color_set, addItem(QString::number(i)));
    }

    // Load animations, sprites & CSRs
    load_animations();
    load_sprites();
    load_csrs();

    // Enable editing
    set_opd_edit_enabled(true);

    // Stop animations if applicable
    if (_in_animation) stop_animation();
}

#define selected_list_item(list)                                               \
    (ui->list->count()) ? ui->list->currentRow() : -1

void MainWindow::reload_ui() {
    // Get current state of animation tree
    int tree_item_at_level_1 = -1;
    int tree_item_at_level_2 = -1;

    const auto tree = ui->tree_animations;
    if (tree->topLevelItemCount()) {
        const auto current_tree_item   = tree->currentItem();
        const auto current_item_parent = current_tree_item->parent();
        if (current_item_parent) {
            tree_item_at_level_1 =
                tree->indexOfTopLevelItem(current_item_parent);
            tree_item_at_level_2 =
                current_item_parent->indexOfChild(current_tree_item);
        } else
            tree_item_at_level_1 = tree->indexOfTopLevelItem(current_tree_item);
    }

    // Get current state of lists
    int list_fp_item = selected_list_item(list_frame_parts);
    int list_hb_item = selected_list_item(list_hitboxes);
    int list_sp_item = selected_list_item(list_sprites);
    int list_cs_item = selected_list_item(list_csrs);

    // Get current scroll values
    const auto tree_scroll = tree->verticalScrollBar()->value();
    const auto list_fp_scroll =
        ui->list_frame_parts->verticalScrollBar()->value();
    const auto list_hb_scroll = ui->list_hitboxes->verticalScrollBar()->value();
    const auto list_sp_scroll = ui->list_sprites->verticalScrollBar()->value();
    const auto list_cs_scroll = ui->list_csrs->verticalScrollBar()->value();

    // Reload ui
    load_ui();

    // Recover current animation state
    if (tree_item_at_level_1 >= tree->topLevelItemCount()) {
        tree_item_at_level_1 = tree->topLevelItemCount() - 1;
        tree_item_at_level_2 = -1;
    }

    if (tree_item_at_level_2 != -1) {
        const auto parent_twi = tree->topLevelItem(tree_item_at_level_1);
        if (tree_item_at_level_2 < parent_twi->childCount()) {
            const auto current_twi = parent_twi->child(tree_item_at_level_2);
            tree->setCurrentItem(current_twi);
        } else {
            tree->setCurrentItem(parent_twi);
            parent_twi->setExpanded(true);
        }
    } else if (tree_item_at_level_1 != -1) {
        const auto current_twi = tree->topLevelItem(tree_item_at_level_1);
        tree->setCurrentItem(current_twi);
        current_twi->setExpanded(true);
    }

    // Recover other states
    if (list_fp_item != -1) ui->list_frame_parts->setCurrentRow(list_fp_item);
    if (list_hb_item != -1) ui->list_hitboxes->setCurrentRow(list_hb_item);
    if (list_sp_item != -1) ui->list_sprites->setCurrentRow(list_sp_item);
    if (list_cs_item != -1) ui->list_csrs->setCurrentRow(list_cs_item);

    // Recover scroll
    tree->verticalScrollBar()->setValue(tree_scroll);
    ui->list_frame_parts->verticalScrollBar()->setValue(list_fp_scroll);
    ui->list_hitboxes->verticalScrollBar()->setValue(list_hb_scroll);
    ui->list_sprites->verticalScrollBar()->setValue(list_sp_scroll);
    ui->list_csrs->verticalScrollBar()->setValue(list_cs_scroll);
}

void MainWindow::set_general_edit_enabled(bool enabled) {
    set_opd_edit_enabled(enabled);
    set_animation_edit_enabled(enabled);
    set_frame_edit_enabled(enabled);
    set_frame_part_edit_enabled(enabled);
    set_hitbox_edit_enabled(enabled);
    set_image_edit_enabled(enabled);
    set_csr_edit_enabled(enabled);
}

void MainWindow::prompt_color_dialog(Color& color) const {
    QColor initial_color { color.r, color.g, color.b };
    QColor new_color = QColorDialog::getColor(initial_color);
    if (new_color.isValid() == false) return;
    color = { (uchar) new_color.red(),
              (uchar) new_color.green(),
              (uchar) new_color.blue() };
}

void MainWindow::save_PNG(const QImage& image) {
    // Get save location
    auto path = QFileDialog::getSaveFileName(
        this, "Export image", _default_image_import_location, "PNG (*.png)"
    );
    if (path.isEmpty()) return;

    // Check extension
    const auto file_name = path.split('/').last();
    const auto file_dir  = path.chopped(file_name.size());
    const auto extension = file_name.split('.').last();
    if (extension.toLower().compare("png") != 0) path += ".png";

    // Save image
    const auto result = image.save(path);
    if (!result) return;

    // Save this path
    _default_image_import_location = file_dir;
    save_app_state();
}

// -----------------------------------------------------------------------------
// Color buttons
// -----------------------------------------------------------------------------

#define COL_SETUP_ALL(button_set, palette)                                     \
    ui->button_set##_0->setup(0, palette);                                     \
    ui->button_set##_1->setup(1, palette);                                     \
    ui->button_set##_2->setup(2, palette);                                     \
    ui->button_set##_3->setup(3, palette);                                     \
    ui->button_set##_4->setup(4, palette);                                     \
    ui->button_set##_5->setup(5, palette);                                     \
    ui->button_set##_6->setup(6, palette);                                     \
    ui->button_set##_7->setup(7, palette);                                     \
    ui->button_set##_8->setup(8, palette);                                     \
    ui->button_set##_9->setup(9, palette);                                     \
    ui->button_set##_A->setup(10, palette);                                    \
    ui->button_set##_B->setup(11, palette);                                    \
    ui->button_set##_C->setup(12, palette);                                    \
    ui->button_set##_D->setup(13, palette);                                    \
    ui->button_set##_E->setup(14, palette);                                    \
    ui->button_set##_F->setup(15, palette)

void MainWindow::setup_color_buttons() {
    // Setup frame part color buttons
    COL_SETUP_ALL(bt_frame_part_col, &_current_part_palette);
    // Setup sprite color buttons
    COL_SETUP_ALL(bt_sprite_col, &_current_sprite_palette);
    // Setup image color buttons
    COL_SETUP_ALL(bt_image_col, &_current_image_palette.display);
    // Setup CSR color buttons
    COL_SETUP_ALL(bt_csr_col, &_current_csr_palette);
}
void MainWindow::update_palettes(
    const uchar color_set, const uchar index, const Color color
) {
    // === UPDATE VALUE ===
    // Update palette
    save_previous_state();
    _opd->palettes[color_set][index] = color;

    // === UPDATE UI ===
    // Update frame part color buttons
    if (ui->cb_frame_part_color_set->currentIndex() == color_set) {
        if (_current_frame_part->index != Invalid::index) {
            _current_part_palette[index] = color;
            bt_frame_part_col_ALL(set_color());
            redraw_frame();
        }
    }
    // Update sprite color buttons
    if (ui->cb_sprite_palette->currentIndex() == color_set) {
        if (_current_sprite->index != Invalid::index) {
            _current_sprite_palette[index] = color;
            bt_sprite_col_ALL(set_color());
            redraw_sprite();
        }
    }
    // Update CSR color buttons
    if (ui->cb_csr_palette->currentIndex() == color_set) {
        if (_current_csr->index != Invalid::index) {
            _current_csr_palette[index] = color;
            bt_csr_col_ALL(set_color());
            redraw_csr();
        }
    }
}

void MainWindow::update_palettes(const uchar color_set, Palette palette) {
    // === UPDATE VALUE ===
    // Update palette
    save_previous_state();
    palette.index             = color_set;
    _opd->palettes[color_set] = palette;

    // === UPDATE UI ===
    // Update frame part color buttons
    if (ui->cb_frame_part_color_set->currentIndex() == color_set) {
        if (_current_frame_part->index != Invalid::index) {
            _current_part_palette = palette;
            bt_frame_part_col_ALL(set_color());
            redraw_frame();
        }
    }
    // Update sprite color buttons
    if (ui->cb_sprite_palette->currentIndex() == color_set) {
        if (_current_sprite->index != Invalid::index) {
            _current_sprite_palette = palette;
            bt_sprite_col_ALL(set_color());
            redraw_sprite();
        }
    }
    // Update CSR color buttons
    if (ui->cb_csr_palette->currentIndex() == color_set) {
        if (_current_csr->index != Invalid::index) {
            _current_csr_palette = palette;
            bt_csr_col_ALL(set_color());
            redraw_csr();
        }
    }
}

// -----------------------------------------------------------------------------
// State timeline
// -----------------------------------------------------------------------------

#include <QDebug>

void MainWindow::save_previous_state() {
    qDebug() << "CHANGE";
    // Clear future states
    while (!_future_states.empty()) {
        delete _future_states.back();
        _future_states.pop_back();
    }
    // Append previous states
    _previous_states.push_back(Opd::copy(_opd));
    // Undo limit
    if (_previous_states.size() > _undo_max) {
        delete _previous_states.front();
        _previous_states.pop_front();
    }
}
void MainWindow::undo() {
    qDebug() << "UNDO";
    if (_previous_states.size() == 0) return;

    // Add future state
    _future_states.push_back(_opd);
    // Get past state
    _opd = _previous_states.back();
    _previous_states.pop_back();

    // Update UI
    reload_ui();
}
void MainWindow::redo() {
    qDebug() << "REDO";
    if (_future_states.size() == 0) return;

    // Add previous state
    _previous_states.push_back(_opd);
    // Get future state
    _opd = _future_states.back();
    _future_states.pop_back();

    // Update UI
    reload_ui();
}

// -----------------------------------------------------------------------------
// (De)Serialization
// -----------------------------------------------------------------------------

#include <fstream>

#define app_state_file "app.state"

#define write_state(state)                                                     \
    file.write(state.toStdString().data(), state.size());                      \
    file.write("\n", 1)
#define read_state(state)                                                      \
    std::getline(file, line);                                                  \
    state = QString::fromStdString(line)

void MainWindow::save_app_state() {
    std::ofstream file { app_state_file, std::ios::out };

    write_state(_default_opd_import_location);
    write_state(_default_image_import_location);
    for (const auto& opd_path : _recently_opd_paths) {
        write_state(opd_path);
    }

    file.close();
}

void MainWindow::load_app_state() {
    std::ifstream file { app_state_file, std::ios::in };
    if (file.is_open() == false) return;

    // Load default locations
    std::string line;
    read_state(_default_opd_import_location);
    read_state(_default_image_import_location);

    // Load recent opd's
    _recently_opd_paths.clear();
    while (!file.eof()) {
        QString recent_opd;
        read_state(recent_opd);
        if (!recent_opd.isEmpty()) //
            _recently_opd_paths.push_back(recent_opd);
    }
    update_recent_menubar();

    file.close();
}