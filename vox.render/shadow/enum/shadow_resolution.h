//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <cstdlib>

namespace vox {
/**
 * Default shadow resolution.
 */
enum ShadowResolution {
    /** Low shadow map resolution. */
    Low,
    /** Medium shadow map resolution. */
    Medium,
    /**  High shadow map resolution. */
    High,
    /** Very high shadow map resolution. */
    VeryHigh
};

}  // namespace vox