#include "main_window.h"
#include "./ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("CHD sprite and palette swapper");
    setAcceptDrops(true);
    load_app_state();

    // Setup color buttons
    ui->bt_col_0->setup(0, &_palette_colors);
    ui->bt_col_1->setup(1, &_palette_colors);
    ui->bt_col_2->setup(2, &_palette_colors);
    ui->bt_col_3->setup(3, &_palette_colors);
    ui->bt_col_4->setup(4, &_palette_colors);
    ui->bt_col_5->setup(5, &_palette_colors);
    ui->bt_col_6->setup(6, &_palette_colors);
    ui->bt_col_7->setup(7, &_palette_colors);
    ui->bt_col_8->setup(8, &_palette_colors);
    ui->bt_col_9->setup(9, &_palette_colors);
    ui->bt_col_A->setup(10, &_palette_colors);
    ui->bt_col_B->setup(11, &_palette_colors);
    ui->bt_col_C->setup(12, &_palette_colors);
    ui->bt_col_D->setup(13, &_palette_colors);
    ui->bt_col_E->setup(14, &_palette_colors);
    ui->bt_col_F->setup(15, &_palette_colors);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initial_load(QImage image) {
    // Enable all disabled functionalities
    set_img_section_enabled(true);

    // Setup palette
    setup_palette(image);

    // Select first image
    QPixmap present_image = QPixmap::fromImage(image);
    present_image         = present_image.scaled(
        512, 512, Qt::KeepAspectRatio, Qt::FastTransformation
    );
    ui->label_image_view->setPixmap(present_image);
}

#include <fstream>

#define app_state_file "app.state"

void MainWindow::save_app_state() {
    std::ofstream file { app_state_file, std::ios::out };

    file.write(
        _default_image_import_location.toStdString().data(),
        _default_image_import_location.size()
    );
    file.write("\n", 1);
    file.write(
        _default_col_import_location.toStdString().data(),
        _default_col_import_location.size()
    );
    file.write("\n", 1);
    file.write(
        _default_csr_import_location.toStdString().data(),
        _default_csr_import_location.size()
    );
    file.close();
}

void MainWindow::load_app_state() {
    std::ifstream file { app_state_file, std::ios::in };

    if (file.is_open() == false) return;

    std::string line;
    std::getline(file, line);
    _default_image_import_location = QString::fromStdString(line);
    std::getline(file, line);
    _default_col_import_location = QString::fromStdString(line);
    std::getline(file, line);
    _default_csr_import_location = QString::fromStdString(line);
}