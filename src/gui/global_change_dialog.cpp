#include "gui/global_change_dialog.h"
#include "../../forms/ui_global_change_dialog.h"

#include "gui/twi/frame_twi.h"

#include <QRegularExpressionValidator>

GlobalChangeDialog::GlobalChangeDialog(Opd* const opd, QWidget* parent)
    : QDialog(parent), ui(new Ui::GlobalChangeDialog), _opd(opd) {
    ui->setupUi(this);
    load_animation_list();
    setup_input_filters();
}
GlobalChangeDialog::~GlobalChangeDialog() {}

// ////////////////////////// //
// GLOBAL CHANGE DIALOG SLOTS //
// ////////////////////////// //

void GlobalChangeDialog::on_bt_current_gcd_mode_clicked() {
    switch (_current_gcd_mode) {
    case GCDMode::Animation:
        _current_gcd_mode = GCDMode::Frame;
        ui->bt_current_gcd_mode->setText("Frame");
        load_frame_list();
        ui->stackedWidget->setCurrentIndex(1);
        return;
    case GCDMode::Frame:
        _current_gcd_mode = GCDMode::Sprite;
        ui->bt_current_gcd_mode->setText("Sprite");
        load_sprite_list();
        ui->stackedWidget->setCurrentIndex(2);
        return;
    case GCDMode::Sprite:
        _current_gcd_mode = GCDMode::Animation;
        ui->bt_current_gcd_mode->setText("Animation");
        load_animation_list();
        ui->stackedWidget->setCurrentIndex(0);
        return;
    }
}

void GlobalChangeDialog::on_bt_apply_clicked() {
    auto index = 0;
    switch (_current_gcd_mode) {
    case GCDMode::Animation: update_animations(); break;
    case GCDMode::Frame: update_frames(); break;
    case GCDMode::Sprite: update_sprites(); break;
    }
    accept();
}
void GlobalChangeDialog::on_bt_cancel_clicked() { close(); }

// //////////////////////////////////// //
// GLOBAL CHANGE DIALOG PRIVATE METHODS //
// //////////////////////////////////// //

void GlobalChangeDialog::load_animation_list() {
    ui->list_gcd_objects->clear();
    for (const auto& animation : _opd->animations) {
        const auto animation_name =
            (animation.name.size())
                ? animation.name
                : "animation " + QString::number(animation.index);
        ui->list_gcd_objects->addItem(animation_name);
    }
}
void GlobalChangeDialog::load_frame_list() {
    ui->list_gcd_objects->clear();
    for (const auto& frame : _opd->frames) {
        auto frame_name = "frame " + QString::number(frame.index);
        if (!frame.name.isEmpty()) frame_name += " (" + frame.name + ")";
        ui->list_gcd_objects->addItem(frame_name);
    }
}
void GlobalChangeDialog::load_sprite_list() {
    ui->list_gcd_objects->clear();
    for (const auto& sprite : _opd->sprites) {
        const auto sprite_name = "Sprite " + QString::number(sprite.index);
        ui->list_gcd_objects->addItem(sprite_name);
    }
}

void GlobalChangeDialog::setup_input_filters() {
    const auto validator = new QRegularExpressionValidator(
        QRegularExpression("^[+*/-]?(?:[1-9][0-9]{0,2}|[0-9])$"), this
    );
    ui->edit_frame_pos_x->setValidator(validator);
    ui->edit_frame_pos_y->setValidator(validator);
    ui->edit_frame_off_x->setValidator(validator);
    ui->edit_frame_off_y->setValidator(validator);
    ui->edit_delay->setValidator(validator);
}

template<typename T>
T modify_value(const QString& modifier, const T value) {
    const auto op = modifier[0];

    if (op.isDigit()) return (T) modifier.toLongLong();

    const auto amount = (T) modifier.right(modifier.size() - 1).toLongLong();
    if (op == '+') return value + amount;
    if (op == '-') return value - amount;
    if (op == '*') return value * amount;
    if (amount == 0) return value;
    if (op == '/') return value / amount;
    return value;
}

#define UPDATE_DATA_WITH(data, modifier)                                       \
    if (modifier.size() > 1) data = modify_value(modifier, data)

void GlobalChangeDialog::update_animations() {
    auto index = 0;
    for (auto& animation : _opd->animations) {
        // Filter only selected rows
        if (!ui->list_gcd_objects->item(index++)->isSelected()) continue;

        // Apply update
        for (auto& frame : animation.frames) {
            UPDATE_DATA_WITH(frame.x_offset, ui->edit_frame_off_x->text());
            UPDATE_DATA_WITH(frame.y_offset, ui->edit_frame_off_y->text());
            UPDATE_DATA_WITH(frame.delay, ui->edit_delay->text());
        }
    }
}
void GlobalChangeDialog::update_frames() {
    auto index = 0;
    for (auto& frame : _opd->frames) {
        // Filter only selected rows
        if (!ui->list_gcd_objects->item(index++)->isSelected()) continue;

        // Apply update
        UPDATE_DATA_WITH(frame.x_offset, ui->edit_frame_pos_x->text());
        UPDATE_DATA_WITH(frame.y_offset, ui->edit_frame_pos_y->text());
    }
}
void GlobalChangeDialog::update_sprites() {
    auto index = 0;
    for (auto& sprite : _opd->sprites) {
        // Filter only selected rows
        if (!ui->list_gcd_objects->item(index++)->isSelected()) continue;

        // Apply update
    }
}