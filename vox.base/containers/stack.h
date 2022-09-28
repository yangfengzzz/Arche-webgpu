//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <stack>

#include "vox.base/containers/deque.h"

namespace vox {
// Redirects std::stack to vox::stack in order to replace std default allocator
// by vox::StdAllocator.
template <class Ty, class Container = typename vox::deque<Ty>>
using stack = std::stack<Ty, Container>;
}  // namespace vox
