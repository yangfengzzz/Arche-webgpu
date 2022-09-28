//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <set>

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::set to vox::set in order to replace std default allocator by
// vox::StdAllocator.
template <class Key, class Pred = std::less<Key>, class Allocator = vox::StdAllocator<Key>>
using set = std::set<Key, Pred, Allocator>;

// Redirects std::multiset to vox::multiset in order to replace std default
// allocator by vox::StdAllocator.
template <class Key, class Pred = std::less<Key>, class Allocator = vox::StdAllocator<Key>>
using multiset = std::multiset<Key, Pred, Allocator>;
}  // namespace vox
