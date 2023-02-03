#include "main_window.h"
#include "./ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("CHD sprite and palette swapper");
    setMinimumSize(774, 606);
    setMaximumSize(774, 606);
    setAcceptDrops(true);
    load_app_state();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::set_editing_enabled(bool is_enabled) {
    ui->label_image_view->setEnabled(is_enabled);
    ui->bt_image_to_clip->setEnabled(is_enabled);
    ui->bt_pallete_to_clip->setEnabled(is_enabled);
    bt_col_ALL(setEnabled(is_enabled));
    if (!is_enabled) {
        bt_col_ALL(setStyleSheet(QString("")));
    }
}

void MainWindow::initial_load(QImage image) {
    // Enable all disabled functionalities
    set_editing_enabled(true);

    // Setup palette
    setup_palette(image);

    // Select first image
    QPixmap present_image = QPixmap::fromImage(image);
    present_image = present_image.scaled(512, 512, Qt::KeepAspectRatio, Qt::FastTransformation);
    ui->label_image_view->setPixmap(present_image);

    _image_list_empty = false;
}

#include <fstream>

#define app_state_file "app.state"

void MainWindow::save_app_state() {
    std::ofstream file { app_state_file, std::ios::out };

    file.write(_default_import_location.toStdString().data(), _default_import_location.size());
    file.close();
}

void MainWindow::load_app_state() {
    std::ifstream file { app_state_file, std::ios::in};

    if (file.is_open() == false) return;

    std::string line;
    std::getline(file, line);
    _default_import_location = QString::fromStdString(line);
}
