//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#ifdef _MSC_VER
#pragma warning(push)
// Removes constant conditional expression warning.
#pragma warning(disable : 4127)
#endif  // _MSC_VER

#include <list>

#ifdef _MSC_VER
#pragma warning(pop)
#endif  // _MSC_VER

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::list to vox::list in order to replace std default allocator by
// vox::StdAllocator.
template <class Ty, class Allocator = vox::StdAllocator<Ty>>
using list = std::list<Ty, Allocator>;
}  // namespace vox
