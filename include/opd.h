#ifndef __OPD_H__
#define __OPD_H__

#include "opd_types.h"

class Opd {
  public:
    ~Opd();

    QLinkedList<GFXPage>   gfx_pages;
    QLinkedList<Sprite>    sprites;
    QLinkedList<Frame>     frames;
    QLinkedList<Animation> animations;

    QLinkedList<Palette> palettes;

    static Opd* open(QString const& path);

  private:
    Opd(const QLinkedList<GFXPage>&   gfx_pages,
        const QLinkedList<Sprite>&    sprites,
        const QLinkedList<Frame>&     frames,
        const QLinkedList<Animation>& animations,
        const QLinkedList<Palette>&   palettes);
};

#endif // __OPD_H__