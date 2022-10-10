//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
// This test spawns a number of high speed objects to check that they don't tunnel through geometry
class PhysicsHighSpeedApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    void createDynamicObject(JPH::Vec3 inPosition,
                             JPH::Vec3 inVelocity,
                             JPH::Shape *inShape,
                             JPH::EMotionQuality inMotionQuality = JPH::EMotionQuality::LinearCast);
    void createDominoBlocks(JPH::Vec3Arg inOffset, int inNumWalls, float inDensity, float inRadius);
    void createFastSmallConvexObjects();
    void createSimpleScene();
    void createConvexOnLargeTriangles();
    void createConvexOnTerrain1();

    static const char *sScenes[];

    static int sSelectedScene;
};
}  // namespace vox