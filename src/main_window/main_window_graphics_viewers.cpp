#include "main_window.h"
#include "../../forms/ui_main_window.h"

#define ICON_MOVE QIcon(":/icons_general/move.png")
#define ICON_EXPAND QIcon(":/icons_general/expand-arrows.png")
#define ICON_AXIS QIcon(":/icons_general/axis.png")
#define ICON_BLANK QIcon(":/icons_general/blank.png")
#define ICON_GRID QIcon(":/icons_general/grid.png")

// ////////////////////////////////// //
// MAIN WINDOW GRAPHICS VIEWERS SLOTS //
// ////////////////////////////////// //

#define mod_is_pan mod.compare("Pan") == 0
#define mod_is_move mod.compare("Move") == 0

#define grid_is_disabled mod.compare("NoGrid") == 0
#define grid_is_xy_axis mod.compare("XYAxis") == 0
#define grid_is_full mod.compare("FullGrid") == 0

void MainWindow::on_bt_reset_view_anim_pressed() { ui->gv_frame->reset_view(); }
void MainWindow::on_bt_grid_visible_anim_pressed() {
    const auto mod = ui->gv_frame->get_current_grid_mod();
    const auto bt  = ui->bt_grid_visible_anim;

    if (grid_is_disabled) {
        bt->setIcon(ICON_AXIS);
        ui->gv_frame->grid_set_xy();
    } else if (grid_is_xy_axis) {
        bt->setIcon(ICON_GRID);
        ui->gv_frame->grid_set_full();
    } else {
        bt->setIcon(ICON_BLANK);
        ui->gv_frame->grid_disable();
    }
}
void MainWindow::on_bt_change_mode_anim_pressed() {
    const auto mod = ui->gv_frame->get_current_control_mod();
    const auto bt  = ui->bt_change_mode_anim;

    if (mod_is_pan) {
        bt->setIcon(ICON_MOVE);
        ui->gv_frame->activate_move([=](short dx, short dy, bool save) {
            if (ui->tool_box_frame->currentIndex() == 0) {
                check_if_valid(_current_frame_part);
                if (save) save_previous_state();

                // Change values
                _current_frame_part->x_offset += dx;
                _current_frame_part->y_offset += dy;

                // Change their display
                change_ui(
                    spin_frame_part_off_x,
                    setValue(ui->spin_frame_part_off_x->value() + dx)
                );
                change_ui(
                    spin_frame_part_off_y,
                    setValue(ui->spin_frame_part_off_y->value() + dy)
                );
            } else {
                check_if_valid(_current_hitbox);
                if (save) save_previous_state();

                // Change values
                _current_hitbox->x_position += dx;
                _current_hitbox->y_position += dy;

                // Change their display
                change_ui(
                    spin_hitbox_pos_x,
                    setValue(ui->spin_hitbox_pos_x->value() + dx)
                );
                change_ui(
                    spin_hitbox_pos_y,
                    setValue(ui->spin_hitbox_pos_y->value() + dy)
                );
            }
            redraw_frame();
        });
    } else {
        bt->setIcon(ICON_EXPAND);
        ui->gv_frame->activate_pan();
    };
}

// //////////////////////////////////////////// //
// MAIN WINDOW GRAPHICS VIEWERS PRIVATE METHODS //
// //////////////////////////////////////////// //