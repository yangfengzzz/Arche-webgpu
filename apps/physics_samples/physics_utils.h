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

public:
    // Create bodies according to method outlined in "FAST SOFTWARE FOR BOX INTERSECTIONS by AFRA ZOMORODIAN" section
    // "The balanced distribution" http://pub.ist.ac.at/~edels/Papers/2002-J-01-FastBoxIntersection.pdf
    static void createBalancedDistribution(JPH::BodyManager* inBodyManager,
                                           int inNumBodies,
                                           float inEnvironmentSize = 512.0f);

public:
    /// Create a mesh shape in the shape of a torus
    /// @param inTorusRadius Radius of the torus ring
    /// @param inTubeRadius Radius of the torus tube
    /// @param inTorusSegments Number of segments around the torus
    /// @param inTubeSegments Number of segments around the tube of the torus
    /// @return The mesh shape
    static JPH::ShapeRefC createTorusMesh(float inTorusRadius,
                                          float inTubeRadius,
                                          uint inTorusSegments = 16,
                                          uint inTubeSegments = 16);
};
}  // namespace vox