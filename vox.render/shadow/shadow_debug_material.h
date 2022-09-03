//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"

namespace vox {
class ShadowDebugMaterial : public BaseMaterial {
public:
    explicit ShadowDebugMaterial(wgpu::Device& device);
};

}  // namespace vox