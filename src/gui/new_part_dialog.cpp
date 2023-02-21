#include "gui/new_part_dialog.h"
#include "../../forms/ui_new_part_dialog.h"

#include "util.h"
#include "gui/lwi/sprite_lwi.h"

NewPartDialog::NewPartDialog(Opd* const opd, QWidget* parent)
    : QDialog(parent), ui(new Ui::NewPartDialog), _opd(opd) {
    ui->setupUi(this);
    initialize_sprite_list();
}
NewPartDialog::~NewPartDialog() {}

// ///////////////////// //
// NEW PART DIALOG SLOTS //
// ///////////////////// //

void NewPartDialog::on_list_sprites_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    const auto sprite_twi = dynamic_cast<SpriteLwi*>(current);
    if (sprite_twi == nullptr) return;
    selected_sprite = sprite_twi->sprite;
    ui->gv_sprite->show_sprite(*selected_sprite, *selected_palette);
}

void NewPartDialog::on_cb_palette_currentIndexChanged(int new_index) {
    selected_palette = _opd->palettes.begin();
    std::advance(selected_palette, new_index);
    ui->gv_sprite->show_sprite(*selected_sprite, *selected_palette);
}

void NewPartDialog::on_bt_add_clicked() { accept(); }
void NewPartDialog::on_bt_cancel_clicked() { close(); }

// /////////////////////////////// //
// NEW PART DIALOG PRIVATE METHODS //
// /////////////////////////////// //

void NewPartDialog::initialize_sprite_list() {
    // Setup palettes
    for (auto i = 1; i < _opd->palettes.size(); i++)
        ui->cb_palette->addItem(QString::number(i));
    selected_palette = _opd->palettes.begin();

    // Setup sprite list
    ForEach(sprite, _opd->sprites)
        ui->list_sprites->addItem(new SpriteLwi(sprite));
    ui->list_sprites->setCurrentRow(0);
}