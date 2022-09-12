//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/component.h"
#include "vox.render/scene.h"

namespace vox {
/**
 * Fog.
 */
class Fog : public Component {
public:
    /**
     * Fog color.
     */
    Color getColor();

    void setColor(const Color& value);

    explicit Fog(Entity* entity);

private:
    void _onEnable() override;

    void _onDisable() override;

private:
    static const std::string _colorProperty;
    Color _color;
};
}  // namespace vox
