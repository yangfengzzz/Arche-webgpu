//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/vector3.h"
#include "vox.simd_math/simd_math.h"

namespace vox {
class Avatar {
public:
    // Root transformation.
    Vector3F root_translation;
    Vector3F root_euler;
    float root_scale = 1.0;

    simd_math::SimdFloat4 kAnkleForward = -simd_math::simd_float4::x_axis();
    simd_math::SimdFloat4 kAnkleUp = simd_math::simd_float4::y_axis();
    simd_math::SimdFloat4 kMidAxis = simd_math::simd_float4::z_axis();
};
}  // namespace vox