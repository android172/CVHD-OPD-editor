#ifndef IMAGE_LWI_H
#define IMAGE_LWI_H

#include <QHash>
#include <QListWidgetItem>

#include "opd/color.h"

// Image list widget item
class ImageLWI : public QListWidgetItem {
  public:
    ImageLWI(
        const QString&           name,
        const QImage&            image,
        QHash<Color, int>* const color_map
    );

    void                           update_color();
    const QVector<QVector<uchar>>& pixels();

    QString name;

  private:
    bool _is_up_to_date = false;

    QHash<Color, int>* const _color_map;
    const QImage             _imported_image;
    QVector<QVector<uchar>>  _pixels;

    void compute_pixels();
};

#endif // IMAGE_LWI_H
