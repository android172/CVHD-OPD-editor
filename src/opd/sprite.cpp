#include "opd/sprite.h"

#include "util.h"

bool row_empty(QVector<uchar>& row);
bool column_empty(QVector<QVector<uchar>>& pixels, uchar column);

// ///////////////////// //
// SPRITE PUBLIC METHODS //
// ///////////////////// //

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
    const QVector<QVector<uchar>>& pixels_in,
    const short                    x_pos_in,
    const short                    y_pos_in,
    const ushort                   width_in,
    const ushort                   height_in
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
            Color color;
            if (color_index <= palette.size) color = palette[color_index];
            else
                color = { (uchar) (color_index * 17),
                          (uchar) (color_index * 17),
                          (uchar) (color_index * 17) };

            // Compute alpha
            uchar alpha = (with_background || color_index) ? 0xff : 0x00;
            alpha *= alpha_multiplier;

            // Set pixel
            QColor q_color { color.r, color.g, color.b, alpha };
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

// ////////////////////// //
// SPRITE PRIVATE METHODS //
// ////////////////////// //

void Sprite::trim_empty_pixels() {
    // Trim height
    while (pixels.size() > 0) {
        if (row_empty(pixels.first())) {
            pixels.pop_front();
            y_pos--;
        } else break;
    }
    while (pixels.size() > 0) {
        if (row_empty(pixels.last())) {
            pixels.pop_back();
        } else break;
    }
    // Trim width
    if (pixels.size() == 0) {
        x_pos = -width;
        return;
    }
    while (pixels[0].size() > 0) {
        if (column_empty(pixels, 0)) {
            for (auto& row : pixels)
                row.pop_front();
            x_pos--;
        } else break;
    }
    while (pixels[0].size() > 0) {
        if (column_empty(pixels, pixels[0].size() - 1)) {
            for (auto& row : pixels)
                row.pop_back();
        } else break;
    }
}

// /////////////////////// //
// SPRITE HELPER FUNCTIONS //
// /////////////////////// //

bool row_empty(QVector<uchar>& row) {
    for (const auto& pixel : row)
        if (pixel != 0) return false;
    return true;
}
bool column_empty(QVector<QVector<uchar>>& pixels, uchar column) {
    for (auto i = 0; i < pixels.size(); i++)
        if (pixels[i][column] != 0) return false;
    return true;
}