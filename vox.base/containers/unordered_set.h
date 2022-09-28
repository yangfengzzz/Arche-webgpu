//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <unordered_set>

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::unordered_set to vox::UnorderedSet in order to replace std
// default allocator by vox::StdAllocator.
template <class Key,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = vox::StdAllocator<Key>>
using unordered_set = std::unordered_set<Key, Hash, KeyEqual, Allocator>;

// Redirects std::unordered_multiset to vox::UnorderedMultiSet in order to
// replace std default allocator by vox::StdAllocator.
template <class Key,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = vox::StdAllocator<Key>>
using unordered_multiset = std::unordered_multiset<Key, Hash, KeyEqual, Allocator>;
}  // namespace vox