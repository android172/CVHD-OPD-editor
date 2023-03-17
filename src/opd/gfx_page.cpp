#include "opd/gfx_page.h"

#include "file_utils.h"

void GFXPage::initialize() {
    // Update pixel grid size
    pixels.resize(height);
    for (auto& row : pixels)
        row.resize(width);

    if (height == 0 || width == 0) return;

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

QImage GFXPage::to_image(const Palette& palette, bool with_background) const {

    QImage image = QImage(width, height, QImage::Format_RGBA8888);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Compute color index
            const auto color_index = pixels[i][j];

            // Get color from index
            const auto color = palette.get_color(color_index);

            // Compute alpha
            const uchar alpha = (with_background || color_index) ? 0xff : 0x00;

            // Set pixel
            const QColor q_color { color.r, color.g, color.b, alpha };
            image.setPixel(j, i, q_color.rgba());
        }
    }

    return image;
}

void GFXPage::save() const {
    // Open new file
    const auto    path = dir + name.toLower();
    std::ofstream csr_file { path.toStdString(),
                             std::ios::out | std::ios::binary };
    if (!csr_file.is_open())
        return throw std::runtime_error(
            "CSR file at path: \"" + path.toStdString() + "\" not found."
        );

    // Unknown header
    write_type(csr_file, (uchar) 0x03);
    write_type(csr_file, (uchar) 0x74);
    fill_zeros(csr_file, 30);
    write_type(csr_file, width);
    write_type(csr_file, height);
    fill_zeros(csr_file, 0x230 - csr_file.tellp());

    // Write all pixels
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width / 2; j++) {
            const uchar pixel =
                (pixels[i][2 * j] << 4) | (pixels[i][2 * j + 1]);
            write_type(csr_file, pixel);
        }
    }

    csr_file.close();
}