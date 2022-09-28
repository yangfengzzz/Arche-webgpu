//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <queue>

#include "vox.base/containers/deque.h"

namespace vox {
// Redirects std::queue to vox::queue in order to replace std default allocator
// by vox::StdAllocator.
template <class Ty, class Container = deque<Ty>>
using queue = std::queue<Ty, Container>;

// Redirects std::priority_queue to vox::priority_queue in order to replace std
// default allocator by vox::StdAllocator.
template <class Ty, class Container = deque<Ty>, class Pred = std::less<typename Container::value_type>>
using priority_queue = std::priority_queue<Ty, Container, Pred>;
}  // namespace vox
