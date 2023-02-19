#include "gui/palette_button.h"

void PaletteButton::setup(const int color_index, Palette* const palette) {
    this->color_index = color_index;
    this->_palette    = palette;
}
void PaletteButton::set_color() {
    if (color_index >= _palette->size) return;
    setStyleSheet(
        "background-color: #" + (*_palette)[color_index].to_hex() + ";"
    );
    update();
}