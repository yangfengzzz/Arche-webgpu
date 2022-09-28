//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

#include "vox.base/containers/std_allocator.h"

namespace vox {
// Redirects std::basic_string to vox::string in order to replace std default
// allocator by vox::StdAllocator.
using string = std::basic_string<char, std::char_traits<char>, vox::StdAllocator<char>>;
}  // namespace vox