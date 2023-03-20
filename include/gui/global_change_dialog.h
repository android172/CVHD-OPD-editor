#ifndef __GLOBAL_CHANGE_DIALOG_H__
#define __GLOBAL_CHANGE_DIALOG_H__

#include <QDialog>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include "opd/animation.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class GlobalChangeDialog;
}
QT_END_NAMESPACE

class GlobalChangeDialog : public QDialog {
    Q_OBJECT
  public:
    explicit GlobalChangeDialog(
        const QVector<QTreeWidgetItem*>& animation_list,
        QWidget*                         parent = nullptr
    );
    ~GlobalChangeDialog();

  private slots:
    void on_bt_apply_clicked();
    void on_bt_cancel_clicked();

  private:
    Ui::GlobalChangeDialog* ui;

    const QVector<QTreeWidgetItem*>& _animation_list {};

    void load_animation_list();
    void setup_input_filters();

    void update_frame_data(AnimationFramePtr frame);
    void update_frame_data(FramePtr frame);
};

#endif // __GLOBAL_CHANGE_DIALOG_H__