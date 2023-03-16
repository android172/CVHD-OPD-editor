#pragma once

#include <QString>

template<typename Container>
typename Container::iterator get_it_at(Container& container, std::size_t i) {
    if (i >= std::size(container)) return std::end(container);
    return std::next(std::begin(container), i);
}

template<typename Container>
typename Container::value_type get_val_at(Container& iterable, uchar index) {
    auto it = iterable.begin();
    for (auto i = 0; i < index; i++)
        ++it;
    return *it;
}

#define ForEach(iterator, iterable)                                            \
    for (auto iterator = iterable.begin(); iterator != iterable.end();         \
         iterator++)