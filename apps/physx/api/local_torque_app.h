//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
class LocalTorqueApp : public ForwardApplication {
public:
    const std::string desc =
            "Tests that the AddLocalTorque function works as expected. "
            "Set the angular damping to zero in each plugin's UI "
            "if you want the angular velocity to remain constant. "
            "That way you can easily see which velocity has been reached in each engine, for the same torque.";

    void loadScene() override;
};
}  // namespace vox