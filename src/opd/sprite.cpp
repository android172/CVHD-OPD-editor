#include "opd/sprite.h"

bool Sprite::operator==(const Sprite& other) const {
    if (gfx_page != other.gfx_page) return false;
    if (gfx_x_pos != other.gfx_x_pos) return false;
    if (gfx_y_pos != other.gfx_y_pos) return false;
    if (width != other.width) return false;
    if (height != other.height) return false;
    return true;
}

void Sprite::initialize() {
    // Update width and height
    if (width != _previous_width || height != _previous_height) {
        _previous_height = height;
        _previous_width  = width;
        pixels.resize(height);
        for (auto& row : pixels)
            row.resize(width);
    }

    // Set pixels
    for (auto i = 0; i < height; i++) {
        const auto y_pos = gfx_y_pos + i;
        for (auto j = 0; j < width; j++) {
            const auto x_pos = gfx_x_pos + j;
            pixels[i][j]     = gfx_page->pixels[y_pos][x_pos];
        }
    }
}