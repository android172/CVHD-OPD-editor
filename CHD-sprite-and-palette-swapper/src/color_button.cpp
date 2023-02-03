#include "color_button.h"

#include <QMouseEvent>

void ColorButton::setup(
    const int color_index, QVector<ColorPair>* const palette_colors
) {
    this->color_index = color_index;
    _palette_colors   = palette_colors;
}

void ColorButton::set_color() {
    if (color_index < _palette_colors->size())
        setStyleSheet(
            QString("background-color: #") +
            (*_palette_colors)[color_index].display.to_hex() + QString(";")
        );
    update();
}

void ColorButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton) emit right_clicked();
    else QPushButton::mousePressEvent(event);
}
