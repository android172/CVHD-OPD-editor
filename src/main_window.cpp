#include "main_window.h"
#include "./ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    setWindowTitle("CHD sprite and palette swapper");
    setAcceptDrops(true);
    load_app_state();

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