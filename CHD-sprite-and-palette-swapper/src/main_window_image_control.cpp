#include "./ui_main_window.h"
#include "main_window.h"

#include "image_lwi.h"

#include <iostream>

#include <QClipboard>
#include <QFileDialog>

const QVector<QString> MainWindow::supported_image_formats {
    "png", "jpg", "jpeg", "jpe", "bmp", "tif", "tiff", "tga"
};

QString all_supported_formats() {
    QString ret = "Images (";

    for (const auto& format : MainWindow::supported_image_formats) {
        ret += QString("*.") + format + " ";
    }
    ret.chop(2);
    ret += ")";
    return ret;
}

void MainWindow::on_bt_import_images_clicked() {
    auto file_list = QFileDialog::getOpenFileNames(
        this, "Import images", _default_import_location, all_supported_formats()
    );
    import_images(file_list);
}

void MainWindow::on_bt_delete_selected_clicked() {
    auto il = ui->list_images;

    // Delete current
    auto current_image = il->currentIndex();
    if (!current_image.isValid()) return;
    il->takeItem(current_image.row());

    // Update rest if last
    if (il->count() == 0) {
        set_editing_enabled(false);
        ui->label_image_view->clear();
        _image_list_empty = true;
    }
}

void MainWindow::on_bt_delete_all_clicked() {
    // Delete all
    ui->list_images->clear();

    // Update other elements
    set_editing_enabled(false);
    ui->label_image_view->clear();
    _image_list_empty = true;
}

void MainWindow::on_list_images_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    auto iv = ui->label_image_view;
    auto li = dynamic_cast<ImageLWI*>(current);

    if (!li) return;

    // Set selected image
    auto   pixels = li->pixels();
    auto   width  = pixels.size();
    auto   height = pixels[0].size();
    QImage image  = QImage(width, height, QImage::Format_RGB32);

    for (int i = 0; i < width; ++i) {
        for (int j = 0; j < height; ++j) {
            Color  c = _palette_colors[pixels[i][j]].display;
            QColor color { c.r, c.g, c.b };
            image.setPixel(j, i, color.rgb());
        }
    }

    QPixmap present_image = QPixmap::fromImage(image);
    present_image         = present_image.scaled(
        512, 512, Qt::KeepAspectRatio, Qt::FastTransformation
    );
    iv->setPixmap(present_image);
}

void MainWindow::on_bt_image_to_clip_clicked() {
    auto current_image =
        dynamic_cast<ImageLWI*>(ui->list_images->currentItem());

    if (!current_image) return;

    QString image_hex {};

    for (const auto& row : current_image->pixels()) {
        for (const auto& pixel : row) {
            char p = pixel;
            if (p > 9) p += 'A' - 10;
            else p += '0';
            image_hex += p;
            image_hex += ' ';
        }
        image_hex[image_hex.size() - 1] = '\n';
    }
    image_hex.chop(1);

    QClipboard* clipboard = QGuiApplication::clipboard();
    clipboard->setText(image_hex);
}

// Import
void MainWindow::import_images(QStringList path_list) {
    // Image list
    auto il = ui->list_images;

    for (auto& file_path : path_list) {
        file_path = "/" + file_path;

        // Get file name
        QStringList splits    = file_path.split('/');
        QString     file_name = splits[splits.size() - 1];

        // Check size
        auto image = QImage(file_path);
        if (image.width() > 512 || image.height() > 512) continue;

        // Preform initialization if necessary
        if (_image_list_empty) initial_load(image);
        else if (_palette_colors.size() < 16) update_palette(image);

        // Save to image list
        auto image_item = new ImageLWI(file_name, image, &_color_index);
        il->addItem(image_item);
    }

    if (path_list.size() != 0) {
        // Remember last import location
        QStringList splits       = path_list[0].split('/');
        QString     file_name    = splits[splits.size() - 1];
        QString     file_dir     = path_list[0].chopped(file_name.size());
        _default_import_location = file_dir;
        save_app_state();

        ui->list_images->setCurrentRow(0);
    }
}

// Drag and drop

#include <QMimeData>

void MainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mime_data { event->mimeData() };

    if (mime_data->hasUrls() == false) {
        event->ignore();
        return;
    }

    // Filter urls
    QStringList file_list;
    for (int i = 0; i < mime_data->urls().size(); ++i) {
        QUrl url = mime_data->urls().at(i);
        if (url.toString().startsWith("file:///")) {
            QString path      = url.toLocalFile();
            auto    splits    = path.split('.');
            QString extension = splits[splits.size() - 1];
            if (supported_image_formats.contains(extension))
                file_list.append(path);
        }
    }

    // Import files
    import_images(file_list);

    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    event->acceptProposedAction();
}

void MainWindow::dragMoveEvent(QDragMoveEvent* event) {
    event->acceptProposedAction();
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event) { event->accept(); }
