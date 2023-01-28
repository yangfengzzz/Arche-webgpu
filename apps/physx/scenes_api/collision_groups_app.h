//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
class CollisionGroupsApp : public ForwardApplication {
public:
    const std::string desc =
            "Tests that the center-of-mass (COM) local offset is properly taken into account. "
            "The box should rotate on its own if the test succeeds. "
            "The rendered RGB frame indicates the location of the COM.";

    void loadScene() override;
};
}  // namespace vox