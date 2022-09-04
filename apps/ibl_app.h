//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/controls/orbit_control.h"
#include "vox.render/forward_application.h"
#include "vox.render/texture/sampled_texturecube.h"

namespace vox {
class IBLApp : public ForwardApplication {
public:
    bool prepare(Platform &platform) override;

    void loadScene() override;

private:
    SampledTextureCubePtr _cubeMap{nullptr};

    struct Material {
        std::string name;
        Color baseColor;
        float roughness;
        float metallic;

        Material(){};

        Material(std::string n, Color c, float r, float m) : name(n) {
            roughness = r;
            metallic = m;
            baseColor = c;
        };
    };
    std::vector<Material> _materials;

    std::string _path;
    std::array<std::string, 6> _images;
};
}  // namespace vox