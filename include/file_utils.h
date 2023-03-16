#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__

const int  __n__            = 1;
const bool is_little_endian = *(char*) &__n__ == 1;

#include <fstream>
#include <filesystem>
#include <QString>
#include <QVector>

// -----------------------------------------------------------------------------
// ifstream
// -----------------------------------------------------------------------------

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

// -----------------------------------------------------------------------------
// ostream
// -----------------------------------------------------------------------------

inline void skip_over(std::ofstream& file, uint size) {
    file.seekp(size, std::ios::cur);
}

template<typename T>
void write_type(std::ofstream& file, T value) {
    auto size = sizeof(T);
    char buffer[16];

    for (auto i = 0; i < size; i++)
        buffer[i] = *((char*) &value + i);

    if (is_little_endian) {
        for (uchar i = 0; i < size / 2; i++) {
            auto j    = size - 1 - i;
            auto t    = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = t;
        }
    }

    file.write(buffer, size);
}

template<>
inline void write_type<QString>(std::ofstream& file, QString value) {
    // Write length
    write_type(file, (uchar) value.size());

    // Write content
    file.write(value.toStdString().data(), value.size());

    // Skip padding
    skip_over(file, 32 - value.size());
}

// -----------------------------------------------------------------------------
// fstream
// -----------------------------------------------------------------------------

inline void skip_over(std::fstream& file, uint size) {
    file.seekp(size, std::ios::cur);
}

template<typename T>
T read_type(std::fstream& file) {
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
inline QString read_type<QString>(std::fstream& file) {
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
QVector<T> read_vector(std::fstream& file) {
    // Read size
    auto const vector_size = read_type<ushort>(file);
    QVector<T> vector { vector_size };

    // Read elements
    for (int i = 0; i < vector_size; i++)
        vector[i] = read_type<T>(file);
}

template<typename T>
void write_type(std::fstream& file, T value) {
    auto size = sizeof(T);
    char buffer[16];

    for (auto i = 0; i < size; i++)
        buffer[i] = *((char*) &value + i);

    if (is_little_endian) {
        for (uchar i = 0; i < size / 2; i++) {
            auto j    = size - 1 - i;
            auto t    = buffer[i];
            buffer[i] = buffer[j];
            buffer[j] = t;
        }
    }

    file.write(buffer, size);
}

template<>
inline void write_type<QString>(std::fstream& file, QString value) {
    // Write length
    write_type(file, (uchar) value.size());

    // Write content
    file.write(value.toStdString().data(), value.size());

    // Skip padding
    skip_over(file, 32 - value.size());
}

#endif // __FILE_UTILS_H__