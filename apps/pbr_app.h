//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <utility>

#include "vox.render/forward_application.h"
#include "vox.toolkit/controls/orbit_control.h"

namespace vox {
class PBRApp : public ForwardApplication {
public:
    void loadScene() override;

private:
    struct Material {
        std::string name;
        Color baseColor;
        float roughness{};
        float metallic{};

        Material() = default;

        Material(std::string n, Color c, float r, float m) : name(std::move(n)) {
            roughness = r;
            metallic = m;
            baseColor = c;
        };
    };
    std::vector<Material> _materials;
};

}  // namespace vox