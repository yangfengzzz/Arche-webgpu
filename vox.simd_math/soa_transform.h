//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/macros.h"
#include "vox.simd_math/soa_float.h"
#include "vox.simd_math/soa_quaternion.h"

namespace vox::simd_math {

// Stores an affine transformation with separate translation, rotation and scale
// attributes.
struct SoaTransform {
    SoaFloat3 translation;
    SoaQuaternion rotation;
    SoaFloat3 scale;

    static VOX_INLINE SoaTransform identity() {
        const SoaTransform ret = {SoaFloat3::zero(), SoaQuaternion::identity(), SoaFloat3::one()};
        return ret;
    }
};
}  // namespace vox::simd_math