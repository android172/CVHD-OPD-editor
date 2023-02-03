#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QPushButton>
#include <QDragEnterEvent>
#include <QStandardPaths>

#include "Color.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    static const QVector<QString> supported_image_formats;

private slots:
    // Image control
    void on_bt_import_images_clicked();
    void on_bt_delete_selected_clicked();
    void on_bt_delete_all_clicked();
    void on_list_images_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_bt_image_to_clip_clicked();

    // Drag and drop
    void dropEvent(QDropEvent *);
    void dragEnterEvent(QDragEnterEvent *);
    void dragMoveEvent(QDragMoveEvent *);
    void dragLeaveEvent(QDragLeaveEvent *);

    // Palette control
    void on_bt_col_0_clicked();
    void on_bt_col_1_clicked();
    void on_bt_col_2_clicked();
    void on_bt_col_3_clicked();
    void on_bt_col_4_clicked();
    void on_bt_col_5_clicked();
    void on_bt_col_6_clicked();
    void on_bt_col_7_clicked();
    void on_bt_col_8_clicked();
    void on_bt_col_9_clicked();
    void on_bt_col_A_clicked();
    void on_bt_col_B_clicked();
    void on_bt_col_C_clicked();
    void on_bt_col_D_clicked();
    void on_bt_col_E_clicked();
    void on_bt_col_F_clicked();
    void on_bt_pallete_to_clip_clicked();

private:
    Ui::MainWindow *ui;

    //  Application state info
    bool _image_list_empty = true;
    QString _default_import_location =
            QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QVector<Color> _pallete_colors {};
    QHash<Color, int> _color_index {};

    // State
    void set_editing_enabled(bool is_enabled);
    void initial_load(QImage image);
    void save_app_state();
    void load_app_state();

    // Image control
    void import_images(QStringList path_list);

    // Palette control
    void setup_palette(QImage image);
    void on_pallete_button_clicked(const char& i, QPushButton* button);
};

#define bt_col_ALL(action) \
    ui->bt_col_0->action;      \
    ui->bt_col_1->action;      \
    ui->bt_col_2->action;      \
    ui->bt_col_3->action;      \
    ui->bt_col_4->action;      \
    ui->bt_col_5->action;      \
    ui->bt_col_6->action;      \
    ui->bt_col_7->action;      \
    ui->bt_col_8->action;      \
    ui->bt_col_9->action;      \
    ui->bt_col_A->action;      \
    ui->bt_col_B->action;      \
    ui->bt_col_C->action;      \
    ui->bt_col_D->action;      \
    ui->bt_col_E->action;      \
    ui->bt_col_F->action;      \

#endif // MAINWINDOW_H
