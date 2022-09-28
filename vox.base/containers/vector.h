//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <vector>

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::vector to vox::vector in order to replace std default
// allocator by vox::StdAllocator.
template <class Ty, class Allocator = vox::StdAllocator<Ty>>
using vector = std::vector<Ty, Allocator>;

// Extends std::vector with two functions that gives access to the start and the
// end of its array of elements.

// Returns the mutable start of the array of elements, or nullptr if
// vector's empty.
template <class Ty, class Allocator>
inline Ty* array_begin(std::vector<Ty, Allocator>& _vector) {
    return _vector.data();
}

// Returns the non-mutable begin of the array of elements, or nullptr if
// vector's empty.
template <class Ty, class Allocator>
inline const Ty* array_begin(const std::vector<Ty, Allocator>& _vector) {
    return _vector.data();
}

// Returns the mutable end of the array of elements, or nullptr if
// vector's empty. Array end is one element past the last element of the
// array, it cannot be dereferenced.
template <class Ty, class Allocator>
inline Ty* array_end(std::vector<Ty, Allocator>& _vector) {
    return _vector.data() + _vector.size();
}

// Returns the non-mutable end of the array of elements, or nullptr if
// vector's empty. Array end is one element past the last element of the
// array, it cannot be dereferenced.
template <class Ty, class Allocator>
inline const Ty* array_end(const std::vector<Ty, Allocator>& _vector) {
    return _vector.data() + _vector.size();
}
}  // namespace vox