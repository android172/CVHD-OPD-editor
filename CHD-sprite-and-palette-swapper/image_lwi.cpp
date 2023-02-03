#include "image_lwi.h"

ImageLWI::ImageLWI(QString name, QImage image, QHash<Color, int> color_map)
    : QListWidgetItem(name), name(name) {
    pixels.resize(image.width());
    for (int i = 0; i < image.width(); ++i) {
        pixels[i].resize(image.height());
        for (int j = 0; j < image.height(); ++j) {
            QColor pixel {image.pixel(j, i)};
            Color color = {
                (unsigned char) pixel.red(),
                (unsigned char) pixel.green(),
                (unsigned char) pixel.blue()
            };
            pixels[i][j] = (unsigned char)color_map[color];
        }
    }
}
