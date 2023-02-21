#ifndef __HITBOX_LWI_H__
#define __HITBOX_LWI_H__

#include <QListWidgetItem>

#include "opd/frame.h"

class HitBoxLwi : public QListWidgetItem {
  public:
    const HitBoxPtr hitbox;

    HitBoxLwi(const HitBoxPtr hitbox) : QListWidgetItem(), hitbox(hitbox) {
        compute_name();
    };
    ~HitBoxLwi() {}

    void compute_name() { setText("Hitbox " + QString::number(hitbox->index)); }

  private:
};

#endif // __HITBOX_LWI_H__