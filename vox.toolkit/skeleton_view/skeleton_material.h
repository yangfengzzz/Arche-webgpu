//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/material/base_material.h"

namespace vox {
class BoneMaterial : public BaseMaterial {
public:
    void setBaseColor(const Color& newValue);

    void setSkeletonBuffer(const std::vector<float>& value);

    explicit BoneMaterial(wgpu::Device& device, const std::string& name = "");

private:
    static const std::string _baseColorProp;
    static const std::string _skeletonBufferProp;
};

class JointMaterial : public BaseMaterial {
public:
    void setSkeletonBuffer(const std::vector<float>& value);

    explicit JointMaterial(wgpu::Device& device, const std::string& name = "");

private:
    static const std::string _skeletonBufferProp;
};
}  // namespace vox