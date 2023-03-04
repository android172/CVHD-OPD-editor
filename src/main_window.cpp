#include "main_window.h"
#include "../forms/ui_main_window.h"

#include "gui/twi/frame_twi.h"
#include "gui/twi/animation_twi.h"

#include <QFileDialog>
#include <QColorDialog>

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

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("CHD sprite and palette swapper");
    setAcceptDrops(true);
    load_app_state();

    // Initially disable editing
    set_general_editing_enabled(false);
    set_animation_edit_enabled(false);
    set_frame_edit_enabled(false);
    set_frame_part_edit_enabled(false);
    set_hitbox_edit_enabled(false);
    set_image_edit_enabled(false);

    // Add default with background setting for GV gui items
    ui->gv_image->with_background  = true;
    ui->gv_sprite->with_background = true;

    // Add mode change button where necessary
    const auto bt_change_mode_frame  = add_bt_change_mode(ui->gv_frame);
    const auto bt_change_mode_sprite = add_bt_change_mode(ui->gv_sprite);

    connect(bt_change_mode_frame, &QPushButton::clicked, this, [=]() {
        on_bt_change_mode_clicked(bt_change_mode_frame);
        if (bt_change_mode_frame->isChecked()) on_activate_frame_move_mode();
        else ui->gv_frame->activate_pan();
    });
    connect(bt_change_mode_sprite, &QPushButton::clicked, this, [=]() {
        on_bt_change_mode_clicked(bt_change_mode_sprite);
        if (bt_change_mode_sprite->isChecked()) on_activate_frame_move_mode();
        else ui->gv_frame->activate_pan();
    });

    // TODO: Temporary
    ui->bt_add_animation->setEnabled(false);
    ui->bt_remove_animation->setEnabled(false);

    // Setup frame part color buttons
    COL_SETUP_ALL(bt_frame_part_col, &_current_part_palette);
    // Setup sprite color buttons
    COL_SETUP_ALL(bt_sprite_col, &_current_sprite_palette);
    // Setup image color buttons
    COL_SETUP_ALL(bt_image_col, &_image_palette);
    // Setup image color buttons
    COL_SETUP_ALL(bt_col, &_image_palette);
    // Setup csr color buttons
    COL_SETUP_ALL(bt_icol, &_csr_palette);
}

MainWindow::~MainWindow() { delete ui; }

// ///////////////// //
// MAIN WINDOW SLOTS //
// ///////////////// //

void MainWindow::on_bt_import_opd_clicked() {
    // Load OPD
    const auto opd_path = QFileDialog::getOpenFileName(
        this, "Import CSR file", _default_opd_import_location, "CHD_opd (*.opd)"
    );
    import_opd(opd_path);
}

// /////////////////////////// //
// MAIN WINDOW PRIVATE METHODS //
// /////////////////////////// //

void MainWindow::import_opd(const QString opd_path) {
    if (opd_path.isEmpty()) return;
    _opd = Opd::open(opd_path);

    // Save default_location
    const auto opd_name          = opd_path.split('/').last();
    _default_opd_import_location = opd_path.chopped(opd_name.size());
    save_app_state();

    // Update palette count
    ui->cb_frame_part_color_set->clear();
    ui->cb_frame_part_color_set->addItem("Default");
    for (auto i = 1; i < _opd->palette_count; i++)
        ui->cb_frame_part_color_set->addItem(QString::number(i));

    // Load animations &  sprites
    load_animations();
    load_sprites();

    // Enable editing
    set_general_editing_enabled(true);
}

void MainWindow::update_color(Color& color) const {
    QColor initial_color { color.r, color.g, color.b };
    QColor new_color = QColorDialog::getColor(initial_color);
    if (new_color.isValid() == false) return;
    color = { (uchar) new_color.red(),
              (uchar) new_color.green(),
              (uchar) new_color.blue() };
}

void MainWindow::set_general_editing_enabled(bool enabled) {
    ui->tab_main->setTabEnabled(1, enabled);
    ui->tab_main->setTabEnabled(2, enabled);
    ui->bt_add_frame->setEnabled(enabled);
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

void MainWindow::on_bt_change_mode_clicked(QPushButton* const button) const {
    if (button->isChecked()) button->setIcon(QIcon(":/icons_general/move.png"));
    else button->setIcon(QIcon(":/icons_general/expand-arrows.png"));
}

QPushButton* MainWindow::add_bt_change_mode(
    GraphicsViewer* const graphics_viewer
) const {
    auto bt_change_mode = new QPushButton(graphics_viewer);
    bt_change_mode->setStyleSheet("background: none; border: none;");
    bt_change_mode->setIcon(QIcon(":/icons_general/expand-arrows.png"));
    bt_change_mode->setIconSize(QSize(32, 32));
    bt_change_mode->setCheckable(true);
    bt_change_mode->move(5, 5);
    bt_change_mode->show();
    return bt_change_mode;
}

// TODO: OLD

void MainWindow::initial_load(QImage image) {
    // Enable all disabled functionalities
    set_img_section_enabled(true);

    // Setup palette
    setup_image_palette(image);

    // Select first image
    QPixmap present_image = QPixmap::fromImage(image);
    present_image         = present_image.scaled(
        512, 512, Qt::KeepAspectRatio, Qt::FastTransformation
    );
    ui->label_image_view->setPixmap(present_image);
}

QImage MainWindow::compute_pixel_map(
    const QVector<QVector<uchar>>& pixels, const QVector<ColorPair>& palette
) {
    auto   width  = pixels.size();
    auto   height = pixels[0].size();
    QImage image  = QImage(width, height, QImage::Format_RGB32);

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            auto  pixel_index = pixels[i][j];
            Color c;
            if (pixel_index < palette.size()) c = palette[pixel_index].display;
            else
                c = { (uchar) (pixel_index * 17),
                      (uchar) (pixel_index * 17),
                      (uchar) (pixel_index * 17) };
            QColor color { c.r, c.g, c.b };
            image.setPixel(j, i, color.rgb());
        }
    }

    return image;
}

#include <fstream>

#define app_state_file "app.state"

#define write_state(state)                                                     \
    file.write(state.toStdString().data(), state.size());                      \
    file.write("\n", 1);
#define read_state(state)                                                      \
    std::getline(file, line);                                                  \
    state = QString::fromStdString(line);

void MainWindow::save_app_state() {
    std::ofstream file { app_state_file, std::ios::out };

    write_state(_default_opd_import_location);
    write_state(_default_image_import_location);
    write_state(_default_col_import_location);
    write_state(_default_csr_import_location);

    file.close();
}

void MainWindow::load_app_state() {
    std::ifstream file { app_state_file, std::ios::in };

    if (file.is_open() == false) return;

    std::string line;
    read_state(_default_opd_import_location);
    read_state(_default_image_import_location);
    read_state(_default_col_import_location);
    read_state(_default_csr_import_location);

    file.close();
}