#include "csr_lwi.h"

#include <fstream>

CsrLWI::CsrLWI(const QString& name, const QString& path)
    : QListWidgetItem(name), name(name), path(path) {
    import_data();
}

// ////////////////////// //
// CSR LWL PUBLIC METHODS //
// ////////////////////// //

const QVector<QVector<uchar>>& CsrLWI::pixels() { return _pixels; }
void                           CsrLWI::import_data() {
    // Read buffer
    std::ifstream csr_file { path.toStdString(),
                             std::ios::in | std::ios::binary };
    if (!csr_file.is_open()) return;

    const int pixel_count = 128 * 128 / 2;
    char      buffer[pixel_count];

    csr_file.seekg(0x230);
    csr_file.read(buffer, pixel_count);
    csr_file.close();

    // Assign pixels
    for (int i = 0; i < 128; i++) {
        _pixels[i].resize(128);
        for (int j = 0; j < 64; j++) {
            const uchar pixel = (uchar) buffer[i * 64 + j];
            _pixels[i][2 * j]     = pixel >> 4;
            _pixels[i][2 * j + 1] = pixel & 15;
        }
    }
}