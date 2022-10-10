//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
// This test will make a dynamic body cyle through various shapes
class PhysicChangeShapeApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    bool mActivateAfterSwitch = true;
    JPH::BodyID mBodyID;
    JPH::Array<JPH::RefConst<JPH::Shape>> mShapes;
    float mTime = 0.0f;
    int mShapeIdx = 0;
};
}  // namespace vox