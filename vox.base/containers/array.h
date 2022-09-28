//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <array>

#include "vox.base/macros.h"

namespace vox {
// Redirects std::array to vox::array .
template <class Ty, size_t N>
using array = std::array<Ty, N>;

// Extends std::array with two functions that gives access to the beginning and the
// end of its array of elements.

// Returns the mutable beginning of the array of elements, or nullptr if
// array's empty.
template <class Ty, size_t N>
inline Ty* array_begin(std::array<Ty, N>& _array) {
    return _array.data();
}

// Returns the non-mutable begin of the array of elements, or nullptr if
// array's empty.
template <class Ty, size_t N>
inline const Ty* array_begin(const std::array<Ty, N>& _array) {
    return _array.data();
}

// Returns the mutable end of the array of elements, or nullptr if
// array's empty. Array end is one element past the last element of the
// array, it cannot be dereferenced.
template <class Ty, size_t N>
inline Ty* array_end(std::array<Ty, N>& _array) {
    return _array.data() + N;
}

// Returns the non-mutable end of the array of elements, or nullptr if
// array's empty. Array end is one element past the last element of the
// array, it cannot be dereferenced.
template <class Ty, size_t N>
inline const Ty* array_end(const std::array<Ty, N>& _array) {
    return _array.data() + N;
}
}  // namespace vox