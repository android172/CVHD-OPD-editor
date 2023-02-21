#include "opd/gfx_page.h"

#include <fstream>

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