#ifndef __GFX_PAGE_H__
#define __GFX_PAGE_H__

#include <list>
#include <QImage>

#include "pixel_map.h"
#include "palette.h"

struct GFXPage {
  public:
    ushort  index = (ushort) -1;
    QString name;
    QString dir;
    ushort  width;
    ushort  height;

    PixelMap pixels;

    void   initialize();
    QImage to_image(const Palette& palette, bool with_background = false) const;

    void save() const;
};
typedef std::list<GFXPage>::iterator GFXPagePtr;

#endif // __GFX_PAGE_H__