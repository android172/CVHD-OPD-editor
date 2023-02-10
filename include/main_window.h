#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDir>
#include <QDragEnterEvent>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QPushButton>

#include "color.h"
#include "color_button.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

    static const QVector<QString> supported_image_formats;

  private slots:
    // Image control
    void on_bt_import_images_clicked();
    void on_bt_delete_selected_clicked();
    void on_bt_delete_all_clicked();
    void on_list_images_itemPressed(QListWidgetItem* current);
    void on_list_images_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );

    // Drag and drop
    void dropEvent(QDropEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dragLeaveEvent(QDragLeaveEvent*);

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
    void on_bt_col_0_right_clicked();
    void on_bt_col_1_right_clicked();
    void on_bt_col_2_right_clicked();
    void on_bt_col_3_right_clicked();
    void on_bt_col_4_right_clicked();
    void on_bt_col_5_right_clicked();
    void on_bt_col_6_right_clicked();
    void on_bt_col_7_right_clicked();
    void on_bt_col_8_right_clicked();
    void on_bt_col_9_right_clicked();
    void on_bt_col_A_right_clicked();
    void on_bt_col_B_right_clicked();
    void on_bt_col_C_right_clicked();
    void on_bt_col_D_right_clicked();
    void on_bt_col_E_right_clicked();
    void on_bt_col_F_right_clicked();
    void on_cb_color_set_textActivated(QString color_set);
    // Col
    void on_bt_icol_0_clicked();
    void on_bt_icol_1_clicked();
    void on_bt_icol_2_clicked();
    void on_bt_icol_3_clicked();
    void on_bt_icol_4_clicked();
    void on_bt_icol_5_clicked();
    void on_bt_icol_6_clicked();
    void on_bt_icol_7_clicked();
    void on_bt_icol_8_clicked();
    void on_bt_icol_9_clicked();
    void on_bt_icol_A_clicked();
    void on_bt_icol_B_clicked();
    void on_bt_icol_C_clicked();
    void on_bt_icol_D_clicked();
    void on_bt_icol_E_clicked();
    void on_bt_icol_F_clicked();
    void on_bt_import_col_clicked();
    void on_bt_palette_to_col_clicked();
    void on_bt_save_col_clicked();

    // Csr control
    void on_bt_import_csr_clicked();
    void on_bt_delete_sel_csr_clicked();
    void on_bt_delete_all_csr_clicked();
    void on_bt_image_to_csr_clicked();
    void on_bt_export_csr_clicked();
    void on_list_csrs_itemPressed(QListWidgetItem* current);
    void on_list_csrs_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );

  private:
    Ui::MainWindow* ui;

    //  Application state info
    QString _default_image_import_location = QDir::homePath();
    QString _default_col_import_location   = QDir::homePath();
    QString _default_csr_import_location   = QDir::homePath();
    bool    _image_list_empty              = true;
    bool    _csr_list_empty                = true;
    bool    _csr_image_presented           = false;

    QVector<QVector<ColorPair>> _csr_palettes { 8 };
    QVector<ColorPair>          _csr_palette {};

    QVector<ColorPair> _image_palette {};
    QHash<Color, int>  _color_index {};

    QString _current_col_path       = "";
    uint    _current_col_set        = 0;
    uchar   _current_col_multiplier = 1;

    // State
    void   initial_load(QImage image);
    QImage compute_pixel_map(
        const QVector<QVector<uchar>>& pixels, const QVector<ColorPair>& palette
    );
    void save_app_state();
    void load_app_state();

    // Image control
    void import_images(QStringList path_list);
    void set_img_section_enabled(bool is_enabled);

    // CSR control
    void import_csrs(QStringList path_list);
    void set_csr_section_enabled(bool is_enabled);

    // Image palette control
    void setup_image_palette(QImage image);
    void update_image_palette(QImage image);
    void on_img_palette_button_clicked(ColorButton* button);
    void on_img_palette_button_right_clicked(ColorButton* button);

    // CSR palette control (col control)
    void import_col(QString path);
    void on_csr_palette_button_clicked(ColorButton* button);
};

#define bt_col_ALL(action)                                                     \
    ui->bt_col_0->action;                                                      \
    ui->bt_col_1->action;                                                      \
    ui->bt_col_2->action;                                                      \
    ui->bt_col_3->action;                                                      \
    ui->bt_col_4->action;                                                      \
    ui->bt_col_5->action;                                                      \
    ui->bt_col_6->action;                                                      \
    ui->bt_col_7->action;                                                      \
    ui->bt_col_8->action;                                                      \
    ui->bt_col_9->action;                                                      \
    ui->bt_col_A->action;                                                      \
    ui->bt_col_B->action;                                                      \
    ui->bt_col_C->action;                                                      \
    ui->bt_col_D->action;                                                      \
    ui->bt_col_E->action;                                                      \
    ui->bt_col_F->action;

#define bt_icol_ALL(action)                                                    \
    ui->bt_icol_0->action;                                                     \
    ui->bt_icol_1->action;                                                     \
    ui->bt_icol_2->action;                                                     \
    ui->bt_icol_3->action;                                                     \
    ui->bt_icol_4->action;                                                     \
    ui->bt_icol_5->action;                                                     \
    ui->bt_icol_6->action;                                                     \
    ui->bt_icol_7->action;                                                     \
    ui->bt_icol_8->action;                                                     \
    ui->bt_icol_9->action;                                                     \
    ui->bt_icol_A->action;                                                     \
    ui->bt_icol_B->action;                                                     \
    ui->bt_icol_C->action;                                                     \
    ui->bt_icol_D->action;                                                     \
    ui->bt_icol_E->action;                                                     \
    ui->bt_icol_F->action;

#endif // MAIN_WINDOW_H
