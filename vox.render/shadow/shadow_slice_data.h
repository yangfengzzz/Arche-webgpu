//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/bounding_plane3.h"
#include "vox.math/bounding_sphere.h"
#include "vox.math/matrix4x4.h"
#include "vox.math/vector3.h"

namespace vox {
class ShadowSliceData {
public:
    // center
    Point3F position{};
    float resolution;

    // light view projection matrix
    Matrix4x4F viewMatrix;
    Matrix4x4F projectionMatrix;
    Matrix4x4F viewProjectMatrix;

    // cull info
    std::array<BoundingPlane3F, 10> cullPlanes{};
    uint8_t cullPlaneCount;

    // bounding sphere
    BoundingSphere splitBoundSphere{};
    float sphereCenterZ{};
};

}  // namespace vox