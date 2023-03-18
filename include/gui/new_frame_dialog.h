#ifndef __NEW_FRAME_DIALOG_H__
#define __NEW_FRAME_DIALOG_H__

#include <QDialog>
#include <QListWidgetItem>

#include "opd/opd.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class NewFrameDialog;
}
QT_END_NAMESPACE

class NewFrameDialog : public QDialog {
    Q_OBJECT
  public:
    explicit NewFrameDialog(Opd* const opd, QWidget* parent = nullptr);
    ~NewFrameDialog();

    QVector<FramePtr> selected_frames {};

  private slots:
    void on_list_frames_currentItemChanged(
        QListWidgetItem* current, QListWidgetItem* previous
    );
    void on_bt_add_clicked();
    void on_bt_create_clicked();
    void on_bt_cancel_clicked();

  private:
    Ui::NewFrameDialog* ui;

    Opd* const _opd;

    void initialize_frame_list();
};

#endif // __NEW_FRAME_DIALOG_H__