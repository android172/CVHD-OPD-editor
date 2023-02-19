#ifndef __NEW_PART_DIALOG_H__
#define __NEW_PART_DIALOG_H__

#include <QDialog>
#include <QListWidgetItem>

#include "opd.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class NewPartDialog;
}
QT_END_NAMESPACE

class NewPartDialog : public QDialog {
    Q_OBJECT
  public:
    explicit NewPartDialog(Opd* const opd, QWidget* parent = nullptr);
    ~NewPartDialog();

    SpritePtr  selected_sprite  = Invalid::sprite;
    PalettePtr selected_palette = Invalid::palette;

  private slots:
    void on_list_sprites_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_cb_palette_currentIndexChanged(int new_index);
    void on_bt_add_clicked();
    void on_bt_cancel_clicked();

  private:
    Ui::NewPartDialog* ui;

    Opd* const _opd;

    void initialize_sprite_list();
};

#endif // __NEW_PART_DIALOG_H__