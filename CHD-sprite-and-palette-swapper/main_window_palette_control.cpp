#include "main_window.h"
#include "./ui_main_window.h"

#include <QColorDialog>
#include <QClipboard>

#define set_button_color(button, i) \
    button->setStyleSheet(QString("background-color: #") + _pallete_colors[i].to_hex() + QString(";"));

void MainWindow::on_bt_col_0_clicked() { on_pallete_button_clicked(0, ui->bt_col_0); }
void MainWindow::on_bt_col_1_clicked() { on_pallete_button_clicked(1, ui->bt_col_1); }
void MainWindow::on_bt_col_2_clicked() { on_pallete_button_clicked(2, ui->bt_col_2); }
void MainWindow::on_bt_col_3_clicked() { on_pallete_button_clicked(3, ui->bt_col_3); }
void MainWindow::on_bt_col_4_clicked() { on_pallete_button_clicked(4, ui->bt_col_4); }
void MainWindow::on_bt_col_5_clicked() { on_pallete_button_clicked(5, ui->bt_col_5); }
void MainWindow::on_bt_col_6_clicked() { on_pallete_button_clicked(6, ui->bt_col_6); }
void MainWindow::on_bt_col_7_clicked() { on_pallete_button_clicked(7, ui->bt_col_7); }
void MainWindow::on_bt_col_8_clicked() { on_pallete_button_clicked(8, ui->bt_col_8); }
void MainWindow::on_bt_col_9_clicked() { on_pallete_button_clicked(9, ui->bt_col_9); }
void MainWindow::on_bt_col_A_clicked() { on_pallete_button_clicked(10, ui->bt_col_A); }
void MainWindow::on_bt_col_B_clicked() { on_pallete_button_clicked(11, ui->bt_col_B); }
void MainWindow::on_bt_col_C_clicked() { on_pallete_button_clicked(12, ui->bt_col_C); }
void MainWindow::on_bt_col_D_clicked() { on_pallete_button_clicked(13, ui->bt_col_D); }
void MainWindow::on_bt_col_E_clicked() { on_pallete_button_clicked(14, ui->bt_col_E); }
void MainWindow::on_bt_col_F_clicked() { on_pallete_button_clicked(15, ui->bt_col_F); }

void MainWindow::on_pallete_button_clicked(const char& i, QPushButton* button) {
    if (_pallete_colors.size() <= i) return;

    auto c = _pallete_colors[i];
    QColor  initial_color {c.r,c.g,c.b};

    QColor new_color = QColorDialog::getColor(initial_color);
    if (new_color.isValid() == false) return;
    Color nc {
        (unsigned char) new_color.red(),
        (unsigned char) new_color.green(),
        (unsigned char) new_color.blue()
    };
    _pallete_colors[i] = nc;
    _color_index.remove(c);
    _color_index[nc] = i;

    // Setup button colors
    set_button_color(button, i);
    button->update();

    // Reload drawn image
    on_list_images_currentItemChanged(ui->list_images->currentItem(), nullptr);
}

void MainWindow::on_bt_pallete_to_clip_clicked() {
    QString pallete_hex {};
    for (const auto& color : _pallete_colors) {
        Color c = color;
        c.r /= 8;
        c.g /= 8;
        c.b /= 8;
        QString rgb = c.to_hex();
        QString bgr = rgb.mid(4, 2) + QString(" ") + rgb.mid(2, 2) + QString(" ") + rgb.chopped(4);
        pallete_hex +=  bgr + QString(" 00 ");
    }
    pallete_hex.chop(1);

    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setText(pallete_hex);
}

void MainWindow::setup_palette(QImage image) {
    // Compute pallete
    _pallete_colors.clear();
    _color_index.clear();
    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            QColor pixel {image.pixel(j,i)};
            Color color {
                (unsigned char) pixel.red(),
                (unsigned char) pixel.green(),
                (unsigned char) pixel.blue()
            };

            if (_pallete_colors.contains(color) == false)
                _pallete_colors.push_back(color);
        }
    }
    _pallete_colors.resize(16);
    for (int i = 0; i < 16; ++i)
        _color_index[_pallete_colors[i]] = i;

    // Setup button colors
    set_button_color(ui->bt_col_0, 0);
    set_button_color(ui->bt_col_1, 1);
    set_button_color(ui->bt_col_2, 2);
    set_button_color(ui->bt_col_3, 3);
    set_button_color(ui->bt_col_4, 4);
    set_button_color(ui->bt_col_5, 5);
    set_button_color(ui->bt_col_6, 6);
    set_button_color(ui->bt_col_7, 7);
    set_button_color(ui->bt_col_8, 8);
    set_button_color(ui->bt_col_9, 9);
    set_button_color(ui->bt_col_A, 10);
    set_button_color(ui->bt_col_B, 11);
    set_button_color(ui->bt_col_C, 12);
    set_button_color(ui->bt_col_D, 13);
    set_button_color(ui->bt_col_E, 14);
    set_button_color(ui->bt_col_F, 15);
    bt_col_ALL(update());
}

