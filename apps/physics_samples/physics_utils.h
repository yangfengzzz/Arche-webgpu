//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/physics/physics_manager.h"

namespace vox {
class PhysicsUtils {
public:
    static constexpr float worldScale = 1.0;

    // Utility function to create a static floor body
    static JPH::Body& createFloor(JPH::BodyInterface& mBodyInterface, float inSize = 50.0f);

    // Utility function to create a floor consisting of very large triangles
    static JPH::Body& createLargeTriangleFloor(JPH::BodyInterface& mBodyInterface);

    // Create an uneven terrain floor body
    static JPH::Body& createMeshTerrain(JPH::BodyInterface& mBodyInterface);
    static JPH::Body& createHeightFieldTerrain(JPH::BodyInterface& mBodyInterface);
};
}  // namespace vox