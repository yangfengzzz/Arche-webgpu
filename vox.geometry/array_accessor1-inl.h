//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.base/macros.h"
#include "vox.base/parallel.h"

namespace vox {

template <typename T>
ArrayAccessor<T, 1>::ArrayAccessor() : _size(0), _data(nullptr) {}

template <typename T>
ArrayAccessor<T, 1>::ArrayAccessor(size_t size, T *const data) {
    reset(size, data);
}

template <typename T>
ArrayAccessor<T, 1>::ArrayAccessor(const ArrayAccessor &other) {
    set(other);
}

template <typename T>
void ArrayAccessor<T, 1>::set(const ArrayAccessor &other) {
    reset(other._size, other._data);
}

template <typename T>
void ArrayAccessor<T, 1>::reset(size_t size, T *const data) {
    _size = size;
    _data = data;
}

template <typename T>
T &ArrayAccessor<T, 1>::at(size_t i) {
    VOX_ASSERT(i < _size);
    return _data[i];
}

template <typename T>
const T &ArrayAccessor<T, 1>::at(size_t i) const {
    VOX_ASSERT(i < _size);
    return _data[i];
}

template <typename T>
T *ArrayAccessor<T, 1>::begin() const {
    return _data;
}

template <typename T>
T *ArrayAccessor<T, 1>::end() const {
    return _data + _size;
}

template <typename T>
T *ArrayAccessor<T, 1>::begin() {
    return _data;
}

template <typename T>
T *ArrayAccessor<T, 1>::end() {
    return _data + _size;
}

template <typename T>
size_t ArrayAccessor<T, 1>::size() const {
    return _size;
}

template <typename T>
T *ArrayAccessor<T, 1>::data() const {
    return _data;
}

template <typename T>
void ArrayAccessor<T, 1>::swap(ArrayAccessor &other) {
    std::swap(other._data, _data);
    std::swap(other._size, _size);
}

template <typename T>
template <typename Callback>
void ArrayAccessor<T, 1>::forEach(Callback func) const {
    for (size_t i = 0; i < size(); ++i) {
        func(at(i));
    }
}

template <typename T>
template <typename Callback>
void ArrayAccessor<T, 1>::forEachIndex(Callback func) const {
    for (size_t i = 0; i < size(); ++i) {
        func(i);
    }
}

template <typename T>
template <typename Callback>
void ArrayAccessor<T, 1>::parallelForEach(Callback func) {
    parallelFor(kZeroSize, size(), [&](size_t i) { func(at(i)); });
}

template <typename T>
template <typename Callback>
void ArrayAccessor<T, 1>::parallelForEachIndex(Callback func) const {
    parallelFor(kZeroSize, size(), func);
}

template <typename T>
T &ArrayAccessor<T, 1>::operator[](size_t i) {
    return _data[i];
}

template <typename T>
const T &ArrayAccessor<T, 1>::operator[](size_t i) const {
    return _data[i];
}

template <typename T>
ArrayAccessor<T, 1> &ArrayAccessor<T, 1>::operator=(const ArrayAccessor &other) {
    set(other);
    return *this;
}

template <typename T>
ArrayAccessor<T, 1>::operator ConstArrayAccessor<T, 1>() const {
    return ConstArrayAccessor<T, 1>(*this);
}

template <typename T>
ConstArrayAccessor<T, 1>::ConstArrayAccessor() : _size(0), _data(nullptr) {}

template <typename T>
ConstArrayAccessor<T, 1>::ConstArrayAccessor(size_t size, const T *const data) {
    _size = size;
    _data = data;
}

template <typename T>
ConstArrayAccessor<T, 1>::ConstArrayAccessor(const ArrayAccessor<T, 1> &other) {
    _size = other.size();
    _data = other.data();
}

template <typename T>
ConstArrayAccessor<T, 1>::ConstArrayAccessor(const ConstArrayAccessor &other) {
    _size = other._size;
    _data = other._data;
}

template <typename T>
const T &ConstArrayAccessor<T, 1>::at(size_t i) const {
    VOX_ASSERT(i < _size);
    return _data[i];
}

template <typename T>
const T *ConstArrayAccessor<T, 1>::begin() const {
    return _data;
}

template <typename T>
const T *ConstArrayAccessor<T, 1>::end() const {
    return _data + _size;
}

template <typename T>
size_t ConstArrayAccessor<T, 1>::size() const {
    return _size;
}

template <typename T>
const T *ConstArrayAccessor<T, 1>::data() const {
    return _data;
}

template <typename T>
template <typename Callback>
void ConstArrayAccessor<T, 1>::forEach(Callback func) const {
    for (size_t i = 0; i < size(); ++i) {
        func(at(i));
    }
}

template <typename T>
template <typename Callback>
void ConstArrayAccessor<T, 1>::forEachIndex(Callback func) const {
    for (size_t i = 0; i < size(); ++i) {
        func(i);
    }
}

template <typename T>
template <typename Callback>
void ConstArrayAccessor<T, 1>::parallelForEachIndex(Callback func) const {
    parallelFor(kZeroSize, size(), func);
}

template <typename T>
const T &ConstArrayAccessor<T, 1>::operator[](size_t i) const {
    return _data[i];
}

}  // namespace vox
