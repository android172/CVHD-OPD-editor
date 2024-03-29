#include "opd/sprite.h"

#include "util.h"

// ///////////////////// //
// SPRITE PUBLIC METHODS //
// ///////////////////// //

bool Sprite::operator==(const Sprite& other) const {
    if (gfx_page->index != other.gfx_page->index) return false;
    if (gfx_x_pos != other.gfx_x_pos) return false;
    if (gfx_y_pos != other.gfx_y_pos) return false;
    if (width != other.width) return false;
    if (height != other.height) return false;
    return true;
}

void Sprite::initialize() {
    // Update width and height
    pixels.resize(height);
    for (auto& row : pixels)
        row.resize(width);
    if (width == 0 || height == 0) return;

    // Set pixels
    for (auto i = 0; i < height; i++) {
        const auto y_pos = gfx_y_pos + i;
        for (auto j = 0; j < width; j++) {
            const auto x_pos = gfx_x_pos + j;
            pixels[i][j]     = gfx_page->pixels[y_pos][x_pos];
        }
    }

    // Trim empty rows and cols
    trim_empty_pixels();
}

void Sprite::from_image(
    const PixelMap& pixels_in,
    const short     x_pos_in,
    const short     y_pos_in,
    const ushort    width_in,
    const ushort    height_in
) {
    // Update width and height
    height = height_in;
    width  = width_in;
    pixels.resize(height);
    for (auto& row : pixels)
        row.resize(width);
    if (width == 0 || height == 0) return;

    // Set pixels
    for (auto i = 0; i < height; i++) {
        const auto y_pos = y_pos_in + i;
        for (auto j = 0; j < width; j++) {
            const auto x_pos = x_pos_in + j;
            if (y_pos < 0 || y_pos >= pixels_in.size() || //
                x_pos < 0 || x_pos >= pixels_in[0].size())
                pixels[i][j] = 0;
            else pixels[i][j] = pixels_in[y_pos][x_pos];
        }
    }

    // Trim empty rows and cols
    trim_empty_pixels();
}

void Sprite::from_frame_parts(const QVector<FramePartPtr>& frame_parts) {
    auto min_x = SHRT_MAX;
    auto min_y = SHRT_MAX;
    auto max_x = SHRT_MIN;
    auto max_y = SHRT_MIN;

    // Compute bounding box
    for (const auto& part : frame_parts) {
        if (part->sprite->width == 0 || part->sprite->height == 0) continue;

        const auto x_left  = part->x_offset + 0;
        const auto x_right = part->x_offset + part->sprite->width;
        const auto y_left  = part->y_offset + 0;
        const auto y_right = part->y_offset + part->sprite->height;

        if (x_left < min_x) min_x = x_left;
        if (y_left < min_y) min_y = y_left;
        if (x_right > max_x) max_x = x_right;
        if (y_right > max_y) max_y = y_right;
    }

    // Compute width and height
    if (max_x <= min_x || max_y <= min_y) return;
    width  = max_x - min_x;
    height = max_y - min_y;

    // Resize pixels & clear
    pixels.resize(height);
    for (auto& row : pixels) {
        row.resize(width);
        for (auto& pixel : row)
            pixel = 0;
    }
    if (width == 0 || height == 0) return;

    // Update pixels
    for (const auto& part : frame_parts) {
        const auto sprite        = part->sprite;
        const auto pixels_height = sprite->pixels.size();
        const auto pixels_width =
            (pixels_height) ? sprite->pixels[0].size() : 0;

        for (auto i = 0; i < sprite->height; i++) {
            // Current sprite position space
            const auto y     = (part->flip_mode & 0b01)
                                   ? sprite->y_pos + (sprite->height - i - 1)
                                   : sprite->y_pos + i;
            // New sprite position space
            const auto y_res = part->y_offset + i - min_y;
            for (auto j = 0; j < sprite->width; j++) {
                const auto x     = (part->flip_mode & 0b10)
                                       ? sprite->x_pos + (sprite->width - j - 1)
                                       : sprite->x_pos + j;
                const auto x_res = part->x_offset + j - min_x;

                // Compute color index of current sprite
                const auto color_index =
                    (x < 0 || y < 0 || x >= pixels_width || y >= pixels_height)
                        ? 0
                        : sprite->pixels[y][x];

                // Dont override with background
                if (color_index != 0) pixels[y_res][x_res] = color_index;
            }
        }
    }

    // Trim empty rows and cols
    trim_empty_pixels();
}

QImage Sprite::to_image(
    const Palette& palette, bool with_background, float alpha_multiplier
) const {
    auto   pixels_height = pixels.size();
    auto   pixels_width  = (pixels_height) ? pixels[0].size() : 0;
    QImage image         = QImage(width, height, QImage::Format_RGBA8888);

    for (int i = 0; i < height; ++i) {
        const auto y = y_pos + i;
        for (int j = 0; j < width; ++j) {
            const auto x = x_pos + j;

            // Compute color index
            const auto color_index =
                (x < 0 || y < 0 || x >= pixels_width || y >= pixels_height)
                    ? 0
                    : pixels[y][x];

            // Get color from index
            const auto color = palette.get_color(color_index);

            // Compute alpha
            const uchar alpha = (with_background || color_index)
                                    ? 0xff * alpha_multiplier
                                    : 0x00;

            // Set pixel
            const QColor q_color { color.r, color.g, color.b, alpha };
            image.setPixel(j, i, q_color.rgba());
        }
    }

    return image;
}

void Sprite::trim() {
    x_pos  = 0;
    y_pos  = 0;
    height = pixels.size();
    width  = (height == 0) ? 0 : pixels[0].size();
}

void Sprite::fill_margin() {
    if (width == 0 || height == 0) return;

    // Fill height
    QVector<uchar> empty_row {};
    if (pixels.size()) {
        for (auto i = 0; i < pixels[0].size(); i++)
            empty_row.push_front(0);
    }
    while (y_pos < 0) {
        pixels.push_front(empty_row);
        y_pos++;
    }
    while (pixels.size() < height)
        pixels.push_back(empty_row);

    // Fill width
    while (x_pos < 0) {
        for (auto& row : pixels)
            row.push_front(0);
        x_pos++;
    }
    while (pixels[0].size() < width) {
        for (auto& row : pixels)
            row.push_back(0);
    }
}

// ////////////////////// //
// SPRITE PRIVATE METHODS //
// ////////////////////// //

void Sprite::trim_empty_pixels() {
    // Trim height
    while (pixels.size() > 0) {
        if (pixels.row_empty(0)) {
            pixels.pop_front();
            y_pos--;
        } else break;
    }
    while (pixels.size() > 0) {
        if (pixels.row_empty(pixels.size() - 1)) {
            pixels.pop_back();
        } else break;
    }
    // Trim width
    if (pixels.size() == 0) {
        x_pos = -width;
        return;
    }
    while (pixels[0].size() > 0) {
        if (pixels.column_empty(0)) {
            for (auto& row : pixels)
                row.pop_front();
            x_pos--;
        } else break;
    }
    while (pixels[0].size() > 0) {
        if (pixels.column_empty(pixels[0].size() - 1)) {
            for (auto& row : pixels)
                row.pop_back();
        } else break;
    }
}