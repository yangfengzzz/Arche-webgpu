//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/fog/fog.h"

namespace vox {
class Exp2Fog : public Fog {
    /**
     * Density of fog.
     */
    [[nodiscard]] float getDensity() const;

    void setDensity(float value);

    explicit Exp2Fog(Entity* entity);

    void _onEnable() override;

    void _onDisable() override;

private:
    float _density = 0.0025;
    static const std::string _densityProperty;
};
}  // namespace vox