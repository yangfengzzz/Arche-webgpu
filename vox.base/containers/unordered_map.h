//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)  // warning C4702: unreachable code
#endif                           // _MSC_VER

#include <unordered_map>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include "vox.base/containers/std_allocator.h"

namespace vox {

// Redirects std::unordered_map to vox::unordered_map in order to replace std
// default allocator by vox::StdAllocator.
template <class Key,
          class Ty,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = vox::StdAllocator<std::pair<const Key, Ty>>>
using unordered_map = std::unordered_map<Key, Ty, Hash, KeyEqual, Allocator>;

// Redirects std::unordered_multimap to vox::UnorderedMultiMap in order to
// replace std default allocator by vox::StdAllocator.
template <class Key,
          class Ty,
          class Hash = std::hash<Key>,
          class KeyEqual = std::equal_to<Key>,
          class Allocator = vox::StdAllocator<std::pair<const Key, Ty>>>
using unordered_multimap = std::unordered_multimap<Key, Ty, Hash, KeyEqual, Allocator>;
}  // namespace vox