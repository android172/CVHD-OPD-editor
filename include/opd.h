#ifndef __OPD_H__
#define __OPD_H__

#include "opd_types.h"

class Opd {
  public:
    ~Opd();

    std::list<GFXPage>&   gfx_pages;
    std::list<Sprite>&    sprites;
    std::list<Frame>&     frames;
    std::list<Animation>& animations;

    std::list<Palette>& palettes;

    static Opd* open(QString const& path);

  private:
    Opd(std::list<GFXPage>&   gfx_pages,
        std::list<Sprite>&    sprites,
        std::list<Frame>&     frames,
        std::list<Animation>& animations,
        std::list<Palette>&   palettes);
};

#endif // __OPD_H__