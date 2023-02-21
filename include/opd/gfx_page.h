#ifndef __GFX_PAGE_H__
#define __GFX_PAGE_H__

#include <QVector>
#include <list>

struct GFXPage {
  public:
    ushort  index = (ushort) -1;
    QString name;
    QString dir;
    ushort  width;
    ushort  height;

    QVector<QVector<uchar>> pixels;

    void initialize();

  private:
    ushort _previous_width  = 0;
    ushort _previous_height = 0;
};
typedef std::list<GFXPage>::iterator GFXPagePtr;

#endif // __GFX_PAGE_H__