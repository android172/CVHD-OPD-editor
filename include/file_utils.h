#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

const int  __n__            = 1;
const bool is_little_endian = *(char*) &__n__ == 1;

#include <fstream>
#include <QString>
#include <QVector>

template<typename T>
T read_type(std::ifstream& file) {
    auto size = sizeof(T);
    char buffer[16];

    file.read(buffer, size);

    if (is_little_endian) {
        for (uchar i = 0; i < size / 2; i++) {
            auto j    = size - 1 - i;
            auto t    = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = t;
        }
    }

    return *((T*) buffer);
}

template<>
inline QString read_type<QString>(std::ifstream& file) {
    // Read length
    auto str_length = read_type<uchar>(file);
    char str_buffer[32];

    // Read content
    file.read(str_buffer, str_length);
    QString str = QString::fromStdString({ str_buffer, str_length });

    // Skip padding
    file.read(str_buffer, 32 - str_length);

    return str;
}

template<typename T>
QVector<T> read_vector(std::ifstream& file) {
    // Read size
    auto const vector_size = read_type<ushort>(file);
    QVector<T> vector { vector_size };

    // Read elements
    for (int i = 0; i < vector_size; i++)
        vector[i] = read_type<T>(file);
}

#endif // __FILE_UTILS_H__