//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <deque>

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::deque to vox::deque in order to replace std default allocator
// by vox::StdAllocator.
template <class Ty, class Allocator = vox::StdAllocator<Ty>>
using deque = std::deque<Ty, Allocator>;
}  // namespace vox
