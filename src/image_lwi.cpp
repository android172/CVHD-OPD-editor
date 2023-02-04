#include "image_lwi.h"

ImageLWI::ImageLWI(
    const QString& name, const QImage& image, QHash<Color, int>* const color_map
)
    : QListWidgetItem(name), name(name), _imported_image(image),
      _color_map(color_map) {
    _pixels.resize(_imported_image.width());
    for (int i = 0; i < _imported_image.width(); ++i)
        _pixels[i].resize(_imported_image.height());
}

void ImageLWI::update_color() { _is_up_to_date = false; }
const QVector<QVector<unsigned char>>& ImageLWI::pixels() {
    if (!_is_up_to_date) compute_pixels();
    return _pixels;
}

void ImageLWI::compute_pixels() {
    for (int i = 0; i < _imported_image.width(); ++i) {
        for (int j = 0; j < _imported_image.height(); ++j) {
            QColor pixel { _imported_image.pixel(j, i) };
            Color  color  = { (unsigned char) pixel.red(),
                              (unsigned char) pixel.green(),
                              (unsigned char) pixel.blue() };
            _pixels[i][j] = (unsigned char) (*_color_map)[color];
        }
    }
    _is_up_to_date = true;
}