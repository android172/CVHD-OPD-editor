#ifndef IMAGELWI_H
#define IMAGELWI_H

#include <QListWidgetItem>

#include "Color.h"

class ImageLWI : public QListWidgetItem {
  public:
    ImageLWI(QString name, QImage image, QHash<Color, int> color_map);

    QString name;
    QVector<QVector<unsigned char>> pixels;
};

#endif // IMAGELWI_H
