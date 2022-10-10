//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"

namespace vox {
// This test creates a number of disjoint piles of blocks to see if the islands are properly determined and that the simulation spreads them out over multiple CPUs
class PhysicsIslandApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;
};
}  // namespace vox