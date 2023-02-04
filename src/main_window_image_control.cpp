#include "./ui_main_window.h"
#include "main_window.h"

#include "image_lwi.h"

#include <iostream>
#include <QClipboard>
#include <QFileDialog>
#include <QMimeData>

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

// /////////////////////// //
// MAIN WINDOW IMAGE SLOTS //
// /////////////////////// //

void MainWindow::on_bt_import_images_clicked() {
    auto file_list = QFileDialog::getOpenFileNames(
        this,
        "Import images",
        _default_image_import_location,
        all_supported_formats()
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
        set_img_section_enabled(false);
        ui->label_image_view->clear();
    }
}

void MainWindow::on_bt_delete_all_clicked() {
    // Delete all
    ui->list_images->clear();

    // Update other elements
    set_img_section_enabled(false);
    ui->label_image_view->clear();
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

// ///////////////////////// //
// MAIN WINDOW DRAG AND DROP //
// ///////////////////////// //

void MainWindow::dropEvent(QDropEvent* event) {
    const QMimeData* mime_data { event->mimeData() };

    if (mime_data->hasUrls() == false) {
        event->ignore();
        return;
    }

    // Filter urls
    QStringList image_paths;
    QStringList csr_paths;
    QString col_path = "";
    for (int i = 0; i < mime_data->urls().size(); ++i) {
        const QUrl url = mime_data->urls().at(i);
        if (url.toString().startsWith("file:///")) {
            const QString path      = url.toLocalFile();
            const QString extension = path.split('.').last();
            if (supported_image_formats.contains(extension))
                image_paths.append(path);
            else if (extension.compare("csr") == 0) csr_paths.append(path);
            else if (extension.compare("col") == 0) col_path = path;
        }
    }

    // Import files
    if (image_paths.size() > 0) import_images(image_paths);
    if (csr_paths.size() > 0) import_csrs(csr_paths);
    if (col_path.size()  > 0) import_col(col_path);

    event->accept();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
    event->acceptProposedAction();
}
void MainWindow::dragMoveEvent(QDragMoveEvent* event) {
    event->acceptProposedAction();
}
void MainWindow::dragLeaveEvent(QDragLeaveEvent* event) { event->accept(); }

// ///////////////////////////////////////// //
// MAIN WINDOW IMAGE CONTROL PRIVATE METHODS //
// ///////////////////////////////////////// //

void MainWindow::import_images(QStringList path_list) {
    // Image list
    auto il = ui->list_images;

    for (auto& file_path : path_list) {
        // Get file name
        const QString file_name = file_path.split('/').last();

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
        const QString file_name        = path_list[0].split('/').last();
        const QString file_dir         = path_list[0].chopped(file_name.size());
        _default_image_import_location = file_dir;
        save_app_state();

        il->setCurrentRow(0);
    }
}

void MainWindow::set_img_section_enabled(bool is_enabled) {
    _image_list_empty = !is_enabled;
    ui->bt_image_to_csr->setEnabled(is_enabled && !_csr_list_empty);
    ui->label_image_view->setEnabled(is_enabled);
    ui->cb_color_set->setEnabled(is_enabled);
    bt_col_ALL(setEnabled(is_enabled));
    if (!is_enabled) { bt_col_ALL(setStyleSheet(QString(""))); }
}
