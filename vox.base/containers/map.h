//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)  // warning C4702: unreachable code
#endif // _MSC_VER

#include <cstring>
#include <map>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::map to vox::map in order to replace std default allocator by
// vox::StdAllocator.
template <class Key,
          class Ty,
          class Pred = std::less<Key>,
          class Allocator = vox::StdAllocator<std::pair<const Key, Ty>>>
using map = std::map<Key, Ty, Pred, Allocator>;

// Implements a string comparator that can be used by std algorithm like maps.
struct str_less {
    bool operator()(const char* const& _left, const char* const& _right) const { return strcmp(_left, _right) < 0; }
};

// Specializes std::map to use c-string as a key.
template <class Ty, class Allocator = vox::StdAllocator<std::pair<const char* const, Ty>>>
using cstring_map = std::map<const char*, Ty, str_less, Allocator>;

// Redirects std::multimap to vox::MultiMap in order to replace std default
// allocator by vox::StdAllocator.
template <class Key,
          class Ty,
          class Pred = std::less<Key>,
          class Allocator = vox::StdAllocator<std::pair<const Key, Ty>>>
using multimap = std::multimap<Key, Ty, Pred, Allocator>;
}  // namespace vox
