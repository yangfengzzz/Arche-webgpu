//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/forward_application.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
class OminiShadowMapApp : public ForwardApplication {
public:
    void loadScene() override;
};
}  // namespace vox