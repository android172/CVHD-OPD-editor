#include "opd_types.h"

#include <sstream>
#include <fstream>

#include <QDebug>

// -----------------------------------------------------------------------------
// Color
// -----------------------------------------------------------------------------

bool Color::operator==(const Color& other) const {
    return r == other.r && g == other.g && b == other.b;
}

QString Color::to_hex() {
    std::ostringstream red, green, blue;
    red << std::hex << (int) r;
    green << std::hex << (int) g;
    blue << std::hex << (int) b;
    auto result = blue.str();
    if (result.size() < 2) result = "0" + result;
    result = green.str() + result;
    if (result.size() < 4) result = "0" + result;
    result = red.str() + result;
    if (result.size() < 6) result = "0" + result;
    return QString::fromUtf8(result.c_str());
}

uint qHash(const Color& key, uint seed) {
    return ((key.r << 16) | (key.g << 8) | key.b) ^ seed;
}

// -----------------------------------------------------------------------------
// Color pair
// -----------------------------------------------------------------------------

bool ColorPair::operator==(const ColorPair& other) const {
    return original.r == other.original.r && //
           original.g == other.original.g && //
           original.b == other.original.b;
}
bool ColorPair::operator<(const ColorPair& other) const {
    int d = other.original.r - original.r;
    if (d) return d < 0;
    d = other.original.g - original.g;
    if (d) return d < 0;
    d = other.original.b - original.b;
    return d < 0;
}

// -----------------------------------------------------------------------------
// Palette
// -----------------------------------------------------------------------------

Palette::iterator Palette::begin() { return _colors.begin(); }
Palette::iterator Palette::end() { return _colors.end(); }

Palette::const_iterator Palette::begin() const { return _colors.begin(); }
Palette::const_iterator Palette::end() const { return _colors.end(); }

Color& Palette::operator[](size_t index) {
    if (index >= 16) throw std::runtime_error("Index out of range (0 - 15).");
    return _colors[index];
}
const Color& Palette::operator[](size_t index) const {
    if (index >= 16) throw std::runtime_error("Index out of range (0 - 15).");
    return _colors[index];
}

// -----------------------------------------------------------------------------
// GFX
// -----------------------------------------------------------------------------

void GFXPage::initialize() {
    // Update pixel grid size
    if (width != _previous_width || height != _previous_height) {
        _previous_height = height;
        _previous_width  = width;
        pixels.resize(height);
        for (auto& row : pixels)
            row.resize(width);
    }

    // Load csr file
    const auto    path = dir + name.toLower();
    std::ifstream csr_file { path.toStdString(),
                             std::ios::in | std::ios::binary };
    if (!csr_file.is_open())
        return throw std::runtime_error(
            "CSR file at path: \"" + path.toStdString() + "\" not found."
        );

    // Prepare buffer
    QVector<char> buffer;
    buffer.resize(width * height / 2); // 2 pixels per byte

    // Read all data
    csr_file.seekg(0x230);
    csr_file.read(buffer.data(), buffer.size());
    csr_file.close();

    // Assign pixels
    const auto h_width = width / 2;
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < h_width; j++) {
            const uchar pixel    = (uchar) buffer[i * h_width + j];
            pixels[i][2 * j]     = pixel >> 4; // First 4 bits
            pixels[i][2 * j + 1] = pixel & 15; // Last 4 bits
        }
    }
}

// -----------------------------------------------------------------------------
// Frame part
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// Frame
// -----------------------------------------------------------------------------

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