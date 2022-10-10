//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
// Tests that spawns various shapes with the center of mass not in the center of the object
class PhysicsCenterOfMassApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;
};
}  // namespace vox