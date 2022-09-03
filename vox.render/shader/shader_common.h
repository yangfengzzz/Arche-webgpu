//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdint>

namespace vox {
enum class Attributes : uint32_t {
    POSITION = 0,
    NORMAL,
    UV_0,
    TANGENT,
    BI_TANGENT,
    COLOR_0,
    WEIGHTS_0,
    JOINTS_0,
    UV_1,
    UV_2,
    UV_3,
    UV_4,
    UV_5,
    UV_6,
    UV_7,
    TOTAL_COUNT
};

}
