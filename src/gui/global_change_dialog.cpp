#include "gui/global_change_dialog.h"
#include "../../forms/ui_global_change_dialog.h"

#include "gui/twi/frame_twi.h"

#include <QRegularExpressionValidator>

GlobalChangeDialog::GlobalChangeDialog(
    const QVector<QTreeWidgetItem*>& animation_list, QWidget* parent
)
    : QDialog(parent), ui(new Ui::GlobalChangeDialog),
      _animation_list(animation_list) {
    ui->setupUi(this);
    load_animation_list();
    setup_input_filters();
}
GlobalChangeDialog::~GlobalChangeDialog() {}

// ////////////////////////// //
// GLOBAL CHANGE DIALOG SLOTS //
// ////////////////////////// //

void GlobalChangeDialog::on_bt_apply_clicked() {
    auto index = 0;
    for (const auto& animation : _animation_list) {
        const auto current_lwi = ui->list_animations->item(index++);
        if (current_lwi->isSelected()) {
            // Apply to all frames
            for (auto i = 0; i < animation->childCount(); i++) {
                const auto frame_twi =
                    dynamic_cast<FrameTwi*>(animation->child(i));
                if (frame_twi != nullptr) {
                    if (frame_twi->animation_info->index != (ushort) -1)
                        update_frame_data(frame_twi->animation_info);
                    else update_frame_data(frame_twi->frame_info);
                }
            }
        }
    }
    accept();
}
void GlobalChangeDialog::on_bt_cancel_clicked() { close(); }

// //////////////////////////////////// //
// GLOBAL CHANGE DIALOG PRIVATE METHODS //
// //////////////////////////////////// //

void GlobalChangeDialog::load_animation_list() {
    for (const auto& animation : _animation_list)
        ui->list_animations->addItem(animation->text(0));
}

void GlobalChangeDialog::setup_input_filters() {
    const auto validator = new QRegularExpressionValidator(
        QRegularExpression("^[+*/-](?:[1-9][0-9]{0,2}|[0-9])$"), this
    );
    ui->edit_frame_pos_x->setValidator(validator);
    ui->edit_frame_pos_y->setValidator(validator);
    ui->edit_frame_off_x->setValidator(validator);
    ui->edit_frame_off_y->setValidator(validator);
    ui->edit_delay->setValidator(validator);
}

template<typename T>
T modify_value(QString modifier, T value) {
    short amount = modifier.right(modifier.size() - 1).toUShort();
    if (modifier[0] == '+') return value + amount;
    if (modifier[0] == '-') return value - amount;
    if (modifier[0] == '*') return value * amount;
    if (modifier[0] == '/') return value / amount;
    return value;
}

#define UPDATE_DATA_WITH(data, modifier)                                       \
    if (modifier.size() > 1) data = modify_value(modifier, data)

void GlobalChangeDialog::update_frame_data(AnimationFramePtr frame) {
    UPDATE_DATA_WITH(frame->data->x_offset, ui->edit_frame_pos_x->text());
    UPDATE_DATA_WITH(frame->data->y_offset, ui->edit_frame_pos_y->text());
    UPDATE_DATA_WITH(frame->x_offset, ui->edit_frame_off_x->text());
    UPDATE_DATA_WITH(frame->y_offset, ui->edit_frame_off_x->text());
    UPDATE_DATA_WITH(frame->delay, ui->edit_delay->text());
}

void GlobalChangeDialog::update_frame_data(FramePtr frame) {
    UPDATE_DATA_WITH(frame->x_offset, ui->edit_frame_pos_x->text());
    UPDATE_DATA_WITH(frame->y_offset, ui->edit_frame_pos_y->text());
}
