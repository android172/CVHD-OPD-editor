#ifndef IMAGE_LWI_H
#define IMAGE_LWI_H

#include <QHash>
#include <QListWidgetItem>

#include "opd/opd_types.h"

// Image list widget item
class ImageLwi : public QListWidgetItem {
  public:
    ImageLwi(
        const QString&             name,
        const QImage&              image,
        QHash<Color, uchar>* const color_map
    )
        : QListWidgetItem(name), name(name), _imported_image(image),
          _color_map(color_map) {
        _pixels.resize(_imported_image.height());
        for (auto i = 0; i < _imported_image.height(); ++i)
            _pixels[i].resize(_imported_image.width());
    }

    void            update_color() { _is_up_to_date = false; }
    const PixelMap& pixels() {
        if (!_is_up_to_date) compute_pixels();
        return _pixels;
    }

    QString name;

  private:
    bool _is_up_to_date = false;

    QHash<Color, uchar>* const _color_map;
    const QImage               _imported_image;
    PixelMap                   _pixels;

    void compute_pixels() {
        for (auto i = 0; i < _imported_image.height(); ++i) {
            for (auto j = 0; j < _imported_image.width(); ++j) {
                QColor pixel { _imported_image.pixel(j, i) };
                Color  color  = { (uchar) pixel.red(),
                                  (uchar) pixel.green(),
                                  (uchar) pixel.blue() };
                _pixels[i][j] = (uchar) (*_color_map)[color];
            }
        }
        _is_up_to_date = true;
    }
};

#endif // IMAGE_LWI_H
