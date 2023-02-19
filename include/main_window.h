#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDir>
#include <QDragEnterEvent>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QPushButton>

#include "color_button.h"
#include "gui/palette_button.h"
#include "opd.h"

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
    // General
    void on_bt_import_opd_clicked();

    // Animations
    void on_tree_animations_itemPressed(QTreeWidgetItem* current, int);
    void on_tree_animations_currentItemChanged(
        QTreeWidgetItem* current, QTreeWidgetItem* previous
    );
    void on_bt_add_animation_clicked();
    void on_bt_remove_animation_clicked();
    void on_line_animation_name_textEdited(QString new_text);
    void on_slider_animation_speed_valueChanged(int new_value);
    void on_bt_play_animation_clicked();

    // Frame
    void on_tool_box_frame_currentChanged(int current);
    void on_bt_add_frame_clicked();
    void on_bt_remove_frame_clicked();
    void on_bt_frame_up_clicked();
    void on_bt_frame_down_clicked();
    void on_line_frame_name_textEdited(QString new_text);
    void on_spin_frame_pos_x_valueChanged(int new_value);
    void on_spin_frame_pos_y_valueChanged(int new_value);
    void on_spin_frame_off_x_valueChanged(int new_value);
    void on_spin_frame_off_y_valueChanged(int new_value);
    void on_spin_frame_delay_valueChanged(int new_value);

    // Frame part
    void on_list_frame_parts_itemPressed(QListWidgetItem* current);
    void on_list_frame_parts_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_bt_add_frame_part_clicked();
    void on_bt_remove_frame_part_clicked();
    void on_bt_frame_part_up_clicked();
    void on_bt_frame_part_down_clicked();
    void on_spin_frame_part_off_x_valueChanged(int new_value);
    void on_spin_frame_part_off_y_valueChanged(int new_value);
    void on_ch_frame_part_flip_x_toggled(bool new_value);
    void on_ch_frame_part_flip_y_toggled(bool new_value);
    void on_cb_frame_part_color_set_currentIndexChanged(int new_index);
    // Frame palette buttons
    void on_bt_frame_part_col_0_clicked();
    void on_bt_frame_part_col_1_clicked();
    void on_bt_frame_part_col_2_clicked();
    void on_bt_frame_part_col_3_clicked();
    void on_bt_frame_part_col_4_clicked();
    void on_bt_frame_part_col_5_clicked();
    void on_bt_frame_part_col_6_clicked();
    void on_bt_frame_part_col_7_clicked();
    void on_bt_frame_part_col_8_clicked();
    void on_bt_frame_part_col_9_clicked();
    void on_bt_frame_part_col_A_clicked();
    void on_bt_frame_part_col_B_clicked();
    void on_bt_frame_part_col_C_clicked();
    void on_bt_frame_part_col_D_clicked();
    void on_bt_frame_part_col_E_clicked();
    void on_bt_frame_part_col_F_clicked();

    // Hitboxes
    void on_list_hitboxes_itemPressed(QListWidgetItem* current);
    void on_list_hitboxes_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_bt_add_hitbox_clicked();
    void on_bt_remove_hitbox_clicked();
    void on_bt_hitbox_up_clicked();
    void on_bt_hitbox_down_clicked();
    void on_spin_hitbox_pos_x_valueChanged(int new_value);
    void on_spin_hitbox_pos_y_valueChanged(int new_value);
    void on_spin_hitbox_width_valueChanged(int new_value);
    void on_spin_hitbox_height_valueChanged(int new_value);

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

    Opd* _opd;

    AnimationPtr      _current_animation  = Invalid::animation;
    AnimationFramePtr _current_anim_frame = Invalid::animation_frame;
    FramePtr          _current_frame      = Invalid::frame;
    FramePartPtr      _current_frame_part = Invalid::frame_part;
    HitBoxPtr         _current_hitbox     = Invalid::hitbox;
    Palette           _current_palette {};

    // General
    void import_opd(const QString opd_path);
    void update_color(Color& color);
    void set_general_editing_enabled(bool enabled);

    // Animation
    void load_animations();
    void load_animation(const AnimationPtr animation);
    void clear_animation();
    void animate_frame(const Animation& animation, ushort frame_count);
    void stop_animation();
    void set_animation_edit_enabled(bool enabled);

    // Frame
    void load_frame(
        const FramePtr frame_info, const AnimationFramePtr animation_info
    );
    void clear_frame();
    void set_frame_edit_enabled(bool enabled);
    void set_frame_movement_enabled(bool enabled);

    // Frame part
    void load_frame_parts();
    void load_frame_part(const FramePartPtr frame_part);
    void clear_frame_part();
    void load_palette();
    void on_bt_frame_part_col_clicked(PaletteButton* const button);
    void set_frame_part_edit_enabled(bool enabled);
    void set_frame_part_movement_enabled(bool enabled);

    // Hitbox
    void load_hitboxes();
    void load_hitbox(const HitBoxPtr hitbox);
    void clear_hitbox();
    void set_hitbox_edit_enabled(bool enabled);
    void set_hitbox_movement_enabled(bool enabled);

    bool _in_animation = false;

    //  Application state info
    QString _default_opd_import_location   = QDir::homePath();
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

#define bt_frame_part_col_ALL(action)                                          \
    ui->bt_frame_part_col_0->action;                                           \
    ui->bt_frame_part_col_1->action;                                           \
    ui->bt_frame_part_col_2->action;                                           \
    ui->bt_frame_part_col_3->action;                                           \
    ui->bt_frame_part_col_4->action;                                           \
    ui->bt_frame_part_col_5->action;                                           \
    ui->bt_frame_part_col_6->action;                                           \
    ui->bt_frame_part_col_7->action;                                           \
    ui->bt_frame_part_col_8->action;                                           \
    ui->bt_frame_part_col_9->action;                                           \
    ui->bt_frame_part_col_A->action;                                           \
    ui->bt_frame_part_col_B->action;                                           \
    ui->bt_frame_part_col_C->action;                                           \
    ui->bt_frame_part_col_D->action;                                           \
    ui->bt_frame_part_col_E->action;                                           \
    ui->bt_frame_part_col_F->action;

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
