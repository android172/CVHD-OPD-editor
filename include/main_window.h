#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDir>
#include <QDragEnterEvent>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QMainWindow>
#include <QPushButton>

#include "gui/graphics_viewer.h"
#include "gui/palette_button.h"
#include "opd/opd.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

#define APPLY_TO_COL(macro, ...)                                               \
    macro(0, __VA_ARGS__);                                                     \
    macro(1, __VA_ARGS__);                                                     \
    macro(2, __VA_ARGS__);                                                     \
    macro(3, __VA_ARGS__);                                                     \
    macro(4, __VA_ARGS__);                                                     \
    macro(5, __VA_ARGS__);                                                     \
    macro(6, __VA_ARGS__);                                                     \
    macro(7, __VA_ARGS__);                                                     \
    macro(8, __VA_ARGS__);                                                     \
    macro(9, __VA_ARGS__);                                                     \
    macro(A, __VA_ARGS__);                                                     \
    macro(B, __VA_ARGS__);                                                     \
    macro(C, __VA_ARGS__);                                                     \
    macro(D, __VA_ARGS__);                                                     \
    macro(E, __VA_ARGS__);                                                     \
    macro(F, __VA_ARGS__)

#define __COL_SLOT__(i, button, signal) void on_##button##_##i##_##signal()
#define COL_SLOTS(button, signal) APPLY_TO_COL(__COL_SLOT__, button, signal)

class MainWindow : public QMainWindow {
    Q_OBJECT

  public:
    // Static
    static const QVector<QString> supported_image_formats;

    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

  private slots:
    // General
    void on_bt_import_opd_clicked();
    // Drag and drop
    void dropEvent(QDropEvent*);
    void dragEnterEvent(QDragEnterEvent*);
    void dragMoveEvent(QDragMoveEvent*);
    void dragLeaveEvent(QDragLeaveEvent*);

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
    void on_bt_merge_frame_parts_clicked();
    void on_bt_edit_frame_part_clicked();
    void on_spin_frame_part_off_x_valueChanged(int new_value);
    void on_spin_frame_part_off_y_valueChanged(int new_value);
    void on_ch_frame_part_flip_x_toggled(bool new_value);
    void on_ch_frame_part_flip_y_toggled(bool new_value);
    void on_cb_frame_part_color_set_currentIndexChanged(int new_index);
    // Frame palette buttons
    COL_SLOTS(bt_frame_part_col, clicked);

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

    // Sprites
    void on_list_sprites_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_bt_add_sprite_clicked();
    void on_bt_remove_sprite_clicked();
    void on_bt_sprite_trim_clicked();
    void on_bt_merge_layers_clicked();
    void on_bt_save_sprite_clicked();
    void on_ch_background_enabled_toggled(bool new_value);
    void on_slider_transparency_valueChanged(int new_value);
    void on_spin_sprite_pos_x_valueChanged(int new_value);
    void on_spin_sprite_pos_y_valueChanged(int new_value);
    void on_spin_sprite_width_valueChanged(int new_value);
    void on_spin_sprite_height_valueChanged(int new_value);
    void on_cb_sprite_palette_currentIndexChanged(int new_index);
    // Sprite palette buttons
    COL_SLOTS(bt_sprite_col, clicked);

    // Images
    void on_list_images_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_bt_import_image_clicked();
    void on_bt_delete_image_clicked();
    void on_bt_delete_all_images_clicked();
    void on_bt_select_rect_clicked();
    void on_bt_import_selected_clicked();
    void on_bt_import_all_clicked();
    void on_bt_import_palette_clicked();
    void on_bt_image_trim_clicked();
    void on_spin_img_pos_x_valueChanged(int new_value);
    void on_spin_img_pos_y_valueChanged(int new_value);
    void on_spin_img_width_valueChanged(int new_value);
    void on_spin_img_height_valueChanged(int new_value);
    // Image palette buttons
    COL_SLOTS(bt_image_col, clicked);

    // CSRs
    void on_list_csrs_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_bt_export_csr_clicked();
    void on_bt_save_col_clicked();
    void on_cb_csr_palette_currentIndexChanged(int new_index);
    // CSR palette buttons
    COL_SLOTS(bt_csr_col, clicked);

  private:
    Ui::MainWindow* ui;

    // Application state
    // Opd file
    Opd* _opd;

    // Animation state
    bool _in_animation = false;

    // Default load locations
    QString _default_opd_import_location   = QDir::homePath();
    QString _default_image_import_location = QDir::homePath();

    // Currently selected
    GFXPagePtr        _current_csr           = Invalid::gfx_page;
    AnimationPtr      _current_animation     = Invalid::animation;
    AnimationFramePtr _current_anim_frame    = Invalid::animation_frame;
    FramePtr          _current_frame         = Invalid::frame;
    FramePartPtr      _current_frame_part    = Invalid::frame_part;
    HitBoxPtr         _current_hitbox        = Invalid::hitbox;
    SpritePtr         _current_sprite        = Invalid::sprite;
    Sprite*           _current_sprite_import = nullptr;

    // Current palettes
    Palette     _current_part_palette {};
    Palette     _current_sprite_palette {};
    Palette     _current_sprite_import_palette {};
    Palette     _current_csr_palette {};
    PalettePair _current_image_palette {};

    QHash<Color, uchar> _color_index {};

    // General methods
    void set_general_editing_enabled(bool enabled);
    void import_opd(const QString opd_path);
    void prompt_color_dialog(Color& color) const;
    void save_PNG(const QImage& image);
    void on_bt_change_mode_clicked(QPushButton* const button) const;

    QPushButton* add_bt_change_mode( //
        GraphicsViewer* const graphics_viewer
    ) const;

    // Color buttons
    void setup_color_buttons();
    void update_palettes(
        const uchar color_set, const uchar index, const Color color
    );
    void update_palettes(const uchar color_set, const Palette palette);
    // (De)Serialization
    void save_app_state();
    void load_app_state();

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
    void on_activate_frame_move_mode();
    void redraw_frame();

    // Frame part
    void load_frame_parts();
    void load_frame_part(const FramePartPtr frame_part);
    void clear_frame_part();
    void load_frame_part_palette();
    void on_bt_frame_part_col_clicked(PaletteButton* const button);
    void set_frame_part_edit_enabled(bool enabled);
    void set_frame_part_movement_enabled(bool enabled);

    // Hitbox
    void load_hitboxes();
    void load_hitbox(const HitBoxPtr hitbox);
    void clear_hitbox();
    void set_hitbox_edit_enabled(bool enabled);
    void set_hitbox_movement_enabled(bool enabled);

    // Sprite
    void load_sprites();
    void load_sprite(const SpritePtr sprite);
    void clear_sprite();
    void on_bt_sprite_col_clicked(PaletteButton* const button);
    void set_sprite_edit_enabled(bool enabled);
    void on_activate_sprite_move_mode();
    void redraw_sprite();

    // Image
    void load_images(QStringList path_list);
    void on_bt_image_col_clicked(PaletteButton* const button);
    void import_image_as_sprite(
        const PixelMap& pixels,
        const short     x,
        const short     y,
        const ushort    w,
        const ushort    h
    );
    void set_image_edit_enabled(bool enabled);
    void redraw_image(const PixelMap pixels);
    void redraw_image();

    // CSR
    void load_csrs();
    void load_csr(const GFXPagePtr csr);
    void clear_csr();
    void on_bt_csr_col_clicked(PaletteButton* const button);
    void set_csr_edit_enabled(bool enabled);
    void redraw_csr();
};

#define __COL_ACTIVATE__(i, button_set, action) ui->button_set##_##i->action
#define COL_ACTIVATE_ALL(button_set, action)                                   \
    APPLY_TO_COL(__COL_ACTIVATE__, button_set, action)

#define bt_frame_part_col_ALL(action)                                          \
    COL_ACTIVATE_ALL(bt_frame_part_col, action)
#define bt_sprite_col_ALL(action) COL_ACTIVATE_ALL(bt_sprite_col, action)
#define bt_image_col_ALL(action) COL_ACTIVATE_ALL(bt_image_col, action)
#define bt_csr_col_ALL(action) COL_ACTIVATE_ALL(bt_csr_col, action)

#endif // MAIN_WINDOW_H
