#include "main_window.h"
#include "../forms/ui_main_window.h"

#include "gui/twi/frame_twi.h"
#include "gui/twi/animation_twi.h"
#include <QFileDialog>

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

    // TODO: Temporary
    ui->bt_add_animation->setEnabled(false);
    ui->bt_remove_animation->setEnabled(false);

    // Setup frame part color buttons
    ui->bt_frame_part_col_0->setup(0, &_current_part_palette);
    ui->bt_frame_part_col_1->setup(1, &_current_part_palette);
    ui->bt_frame_part_col_2->setup(2, &_current_part_palette);
    ui->bt_frame_part_col_3->setup(3, &_current_part_palette);
    ui->bt_frame_part_col_4->setup(4, &_current_part_palette);
    ui->bt_frame_part_col_5->setup(5, &_current_part_palette);
    ui->bt_frame_part_col_6->setup(6, &_current_part_palette);
    ui->bt_frame_part_col_7->setup(7, &_current_part_palette);
    ui->bt_frame_part_col_8->setup(8, &_current_part_palette);
    ui->bt_frame_part_col_9->setup(9, &_current_part_palette);
    ui->bt_frame_part_col_A->setup(10, &_current_part_palette);
    ui->bt_frame_part_col_B->setup(11, &_current_part_palette);
    ui->bt_frame_part_col_C->setup(12, &_current_part_palette);
    ui->bt_frame_part_col_D->setup(13, &_current_part_palette);
    ui->bt_frame_part_col_E->setup(14, &_current_part_palette);
    ui->bt_frame_part_col_F->setup(15, &_current_part_palette);

    // Setup image color buttons
    ui->bt_col_0->setup(0, &_image_palette);
    ui->bt_col_1->setup(1, &_image_palette);
    ui->bt_col_2->setup(2, &_image_palette);
    ui->bt_col_3->setup(3, &_image_palette);
    ui->bt_col_4->setup(4, &_image_palette);
    ui->bt_col_5->setup(5, &_image_palette);
    ui->bt_col_6->setup(6, &_image_palette);
    ui->bt_col_7->setup(7, &_image_palette);
    ui->bt_col_8->setup(8, &_image_palette);
    ui->bt_col_9->setup(9, &_image_palette);
    ui->bt_col_A->setup(10, &_image_palette);
    ui->bt_col_B->setup(11, &_image_palette);
    ui->bt_col_C->setup(12, &_image_palette);
    ui->bt_col_D->setup(13, &_image_palette);
    ui->bt_col_E->setup(14, &_image_palette);
    ui->bt_col_F->setup(15, &_image_palette);

    // Setup csr color buttons
    ui->bt_icol_0->setup(0, &_csr_palette);
    ui->bt_icol_1->setup(1, &_csr_palette);
    ui->bt_icol_2->setup(2, &_csr_palette);
    ui->bt_icol_3->setup(3, &_csr_palette);
    ui->bt_icol_4->setup(4, &_csr_palette);
    ui->bt_icol_5->setup(5, &_csr_palette);
    ui->bt_icol_6->setup(6, &_csr_palette);
    ui->bt_icol_7->setup(7, &_csr_palette);
    ui->bt_icol_8->setup(8, &_csr_palette);
    ui->bt_icol_9->setup(9, &_csr_palette);
    ui->bt_icol_A->setup(10, &_csr_palette);
    ui->bt_icol_B->setup(11, &_csr_palette);
    ui->bt_icol_C->setup(12, &_csr_palette);
    ui->bt_icol_D->setup(13, &_csr_palette);
    ui->bt_icol_E->setup(14, &_csr_palette);
    ui->bt_icol_F->setup(15, &_csr_palette);
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

#include <QColorDialog>

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
    for (auto i = 1; i < _opd->palettes.size(); i++)
        ui->cb_frame_part_color_set->addItem(QString::number(i));

    // Load animations &  sprites
    load_animations();
    load_sprites();

    // Enable editing
    set_general_editing_enabled(true);
}

void MainWindow::update_color(Color& color) {
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