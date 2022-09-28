//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/quaternion.h"
#include "vox.math/vector3.h"

namespace vox {

// Stores an affine transformation with separate translation, rotation and scale
// attributes.
struct VOX_BASE_DLL ScalableTransform {
    // Translation affine transformation component.
    Vector3F translation;

    // Rotation affine transformation component.
    QuaternionF rotation;

    // Scale affine transformation component.
    Vector3F scale;

    // Builds an identity transform.
    static VOX_INLINE ScalableTransform identity() {
        return {Vector3F(), QuaternionF::makeIdentity(), Vector3F(1, 1, 1)};
    }
};
}  // namespace vox