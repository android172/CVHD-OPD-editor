#pragma once

#include <QLinkedList>

template<typename T>
const typename T::iterator get_it_at(T& iterable, uchar index) {
    auto it = iterable.begin();
    std::advance(it, index);
    return it;
}

template<typename T>
typename T::value_type get_val_at(T& iterable, uchar index) {
    auto it = iterable.begin();
    for (auto i = 0; i < index; i++)
        ++it;
    return *it;
}

#define ForEach(iterable, iterator)                                            \
    for (auto iterator = iterable.begin(); iterator != iterable.end();         \
         iterator++)