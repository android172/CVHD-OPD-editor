#include "opd/frame.h"

#include "opd/sprite.h"

void Frame::initialize() {
    for (auto& part : parts) {
        // Compute max color index used
        auto max_color = 0;
        for (auto& row : part.sprite->pixels) {
            for (auto& pixel : row)
                if (pixel + 1 > max_color) max_color = pixel + 1;
        }
        if (max_color > part.palette->size) part.palette->size = max_color;
    }
}
