#include "gui/new_frame_dialog.h"
#include "../../forms/ui_new_frame_dialog.h"

#include "util.h"
#include "gui/lwi/frame_lwi.h"

NewFrameDialog::NewFrameDialog(Opd* const opd, QWidget* parent)
    : QDialog(parent), ui(new Ui::NewFrameDialog), _opd(opd) {
    ui->setupUi(this);
    initialize_frame_list();
}
NewFrameDialog::~NewFrameDialog() {}

// ////////////////////// //
// NEW FRAME DIALOG SLOTS //
// ////////////////////// //

void NewFrameDialog::on_list_frames_currentItemChanged(
    QListWidgetItem* current, QListWidgetItem* previous
) {
    auto frame_lwi = dynamic_cast<FrameLwi*>(current);
    if (frame_lwi == nullptr) return;
    selected_frame = frame_lwi->frame;
    ui->gv_frame->show_frame(*selected_frame);
}

// //////////////////////////////// //
// NEW FRAME DIALOG PRIVATE METHODS //
// //////////////////////////////// //

void NewFrameDialog::initialize_frame_list() {
    ForEach(frame, _opd->frames) ui->list_frames->addItem(new FrameLwi(frame));
    ui->list_frames->setCurrentRow(0);
}

#define DONE(i)                                                                \
    ui->gv_frame->clear();                                                     \
    done(i)

void NewFrameDialog::on_bt_add_clicked() { DONE(1); }
void NewFrameDialog::on_bt_cancel_clicked() { DONE(0); }
void NewFrameDialog::on_bt_create_clicked() {
    selected_frame = _opd->add_new_frame();
    DONE(2);
}