#include "opd/palette.h"

// -----------------------------------------------------------------------------
// Palette
// -----------------------------------------------------------------------------

Color Palette::get_color(uchar index) const {
    return (index <= size) ? _colors[index]
                           : Color { (uchar) (index * 17),
                                     (uchar) (index * 17),
                                     (uchar) (index * 17) };
}

Palette::iterator Palette::begin() { return _colors.begin(); }
Palette::iterator Palette::end() { return _colors.end(); }

Palette::const_iterator Palette::begin() const { return _colors.begin(); }
Palette::const_iterator Palette::end() const { return _colors.end(); }

bool Palette::contains(const Color color) const {
    for (auto i = 0; i < size; i++)
        if (_colors[i] == color) return true;
    return false;
}

void Palette::add_color(const Color color) {
    if (size >= 16) return;
    _colors[size++] = color;
}

Color& Palette::operator[](size_t index) {
    if (index >= 16) throw std::runtime_error("Index out of range (0 - 15).");
    return _colors[index];
}
const Color& Palette::operator[](size_t index) const {
    if (index >= 16) throw std::runtime_error("Index out of range (0 - 15).");
    return _colors[index];
}

// -----------------------------------------------------------------------------
// Palette pair
// -----------------------------------------------------------------------------

void PalettePair::compute_from_image(
    QImage image, QHash<Color, uchar>& out_color_indices
) {
    auto initial_size = original.size;

    // Compute original palette
    for (int i = 0; i < image.width(); ++i) {
        for (int j = 0; j < image.height(); ++j) {
            const QColor pixel { image.pixel(j, i) };
            const Color  color { (uchar) pixel.red(),
                                (uchar) pixel.green(),
                                (uchar) pixel.blue() };

            if (original.contains(color) == false) {
                original.add_color(color);
                display.add_color(color);
            }
        }
    }

    // Check if there were any changes
    if (original.size == initial_size) return;

    // Sort palette
    Color temp_color {};
    for (int i = 1; i < original.size; ++i) {
        for (int j = 2; j < original.size; ++j) {
            if (original[j] < original[i]) {
                // Swap original
                temp_color  = original[j];
                original[j] = original[i];
                original[i] = temp_color;

                // Swap display
                temp_color = display[j];
                display[j] = display[i];
                display[i] = temp_color;
            }
        }
    }

    // Compute color indices
    out_color_indices.clear();
    for (int i = 0; i < original.size; ++i)
        out_color_indices[original[i]] = i;
}
