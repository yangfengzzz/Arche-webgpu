//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/macros.h"

namespace vox {
// Case-sensitive wildcard string matching:
// - a ? sign matches any character, except an empty string.
// - a * sign matches any string, including an empty string.
VOX_BASE_DLL bool strmatch(const char* _str, const char* _pattern);
}  // namespace vox
