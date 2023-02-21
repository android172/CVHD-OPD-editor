#include "opd/frame.h"

void Frame::initialize() {
    for (auto& part : parts) {
        // Compute max color index used
        auto max_color = 0;
        for (auto& row : part.sprite->pixels) {
            for (auto& pixel : row)
                if (pixel > max_color) max_color = pixel;
        }
        if (max_color > part.palette->size) part.palette->size = max_color;
    }
}
