#ifndef __SPRITE_LWI_H__
#define __SPRITE_LWI_H__

#include <QListWidgetItem>
#include "opd_types.h"

class SpriteLwi : public QListWidgetItem {
  public:
    const SpritePtr sprite;

    SpriteLwi(const SpritePtr sprite) : QListWidgetItem(), sprite(sprite) {
        compute_name();
    }
    ~SpriteLwi() {}

    void compute_name() { setText("Sprite " + QString::number(sprite->index)); }
};

#endif // __SPRITE_LWI_H__