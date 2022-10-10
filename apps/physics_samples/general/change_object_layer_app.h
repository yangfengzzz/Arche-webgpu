//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
// This test will demonstrate how to use layers to disable collisions between other objects and how to change them on
// the fly. The bodies will switch between the MOVING layer and the DEBRIS layer (debris only collides with static).
class PhysicsChangeObjectLayerApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    JPH::BodyID mMoving;
    JPH::BodyIDVector mDebris;
    bool mIsDebris = true;
    float mTime = 0.0f;
};
}  // namespace vox