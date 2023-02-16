#ifndef __COLOR_BUTTON_H__
#define __COLOR_BUTTON_H__

#include <QPushButton>

#include "opd_types.h"

class ColorButton : public QPushButton {
    Q_OBJECT

  public:
    using QPushButton::QPushButton;

    int color_index = 0;

    void setup(const int color_index, QVector<ColorPair>* const palette_colors);
    void set_color();

  private:
    QVector<ColorPair>* _palette_colors {};

  private slots:
    void mousePressEvent(QMouseEvent* e);

  signals:
    void right_clicked();
};

#endif // __COLOR_BUTTON_H__