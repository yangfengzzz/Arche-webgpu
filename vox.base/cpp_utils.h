//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>

namespace vox {

template <class ForwardIt, class T, class Compare = std::less<T>>
ForwardIt binaryFind(ForwardIt first, ForwardIt last, const T &value, Compare comp = {});
}  // namespace vox

#include "vox.base/cpp_utils-inl.h"
