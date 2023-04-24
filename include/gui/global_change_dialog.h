#ifndef __GLOBAL_CHANGE_DIALOG_H__
#define __GLOBAL_CHANGE_DIALOG_H__

#include <QDialog>
#include <QListWidgetItem>

#include "opd/opd.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GlobalChangeDialog;
}
QT_END_NAMESPACE

class GlobalChangeDialog : public QDialog {
    Q_OBJECT
  public:
    explicit GlobalChangeDialog(Opd* const opd, QWidget* parent = nullptr);
    ~GlobalChangeDialog();

  private slots:
    void on_bt_current_gcd_mode_clicked();
    void on_bt_apply_clicked();
    void on_bt_cancel_clicked();

  private:
    Ui::GlobalChangeDialog* ui;

    enum class GCDMode { Animation, Frame, Sprite };
    GCDMode _current_gcd_mode = GCDMode::Animation;

    Opd* const _opd;

    void load_animation_list();
    void load_frame_list();
    void load_sprite_list();

    void setup_input_filters();

    void update_animations();
    void update_frames();
    void update_sprites();
};

#endif // __GLOBAL_CHANGE_DIALOG_H__