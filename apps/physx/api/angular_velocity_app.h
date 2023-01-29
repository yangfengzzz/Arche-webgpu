//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
class AngularVelocityApp : public ForwardApplication {
public:
    const std::string desc =
            "Tests that the SetAngularVelocity and GetAngularVelocity functions work as expected. "
            "Set the angular damping to zero in each plugin's UI if you want the velocity to remain constant. "
            "Otherwise it will decrease at a slightly different rate for each engine, "
            "depending on how damping is implemented. "
            "You can also use this test to check that the 'max angular velocity' setting works.";

    void loadScene() override;
};
}  // namespace vox