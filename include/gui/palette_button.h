#ifndef __PALETTE_BUTTON_H__
#define __PALETTE_BUTTON_H__

#include <QPushButton>
#include "opd/palette.h"

class PaletteButton : public QPushButton {
    Q_OBJECT
  public:
    using QPushButton::QPushButton;

    int color_index = 0;

    void setup(const int color_index, Palette* const palette);
    void set_color();
    void clear_color();

  private:
    Palette* _palette {};
};

#endif // __PALETTE_BUTTON_H__