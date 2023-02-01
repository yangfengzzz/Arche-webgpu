//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
class CollisionGroupApp : public ForwardApplication {
public:
    const std::string desc =
            "Simple filtering test, replicating the old collision groups from the NovodeX SDK. "
            "If the test works, the falling spheres should not collide with the spheres below them. "
            "The groups will mainly be used to disable collision detection between jointed "
            "objects, for engines that do not have a built-in feature for doing so.";

    void loadScene() override;
};
}  // namespace vox