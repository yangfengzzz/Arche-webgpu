//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
// This app tests a body that activates during the simulation step to check if it does collision detection with any other bodies during that step
// To do so it uses 3 boxes that all initially collide. The left most box is the only one awake and has a high velocity.
// The second box should not pass through the third box.
class PhysicsActivateDuringUpdateApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;
};
}  // namespace vox