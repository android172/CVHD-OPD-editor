#ifndef __PIXEL_MAP_H__
#define __PIXEL_MAP_H__

#include <QVector>

class PixelMap : public QVector<QVector<uchar>> {
  public:
    using QVector<QVector<uchar>>::QVector;

    bool row_empty(ushort row) const {
        if (size() <= row) return true;
        for (const auto pixel : (*this)[row])
            if (pixel != 0) return false;
        return true;
    }
    bool column_empty(ushort column) const {
        if (size() == 0 || (*this)[0].size() <= column) return true;
        for (auto i = 0; i < size(); i++)
            if ((*this)[i][column] != 0) return false;
        return true;
    }
};

#endif // __PIXEL_MAP_H__
