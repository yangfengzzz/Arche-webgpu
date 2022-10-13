//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>

namespace vox::nav {
typedef int64_t TimeVal;

TimeVal getPerfTime();

int getPerfTimeUsec(TimeVal duration);
}  // namespace vox::nav