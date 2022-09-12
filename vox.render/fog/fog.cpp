//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "fog.h"

namespace vox {
Color Fog::getColor() { return _color; }

void Fog::setColor(const Color& value) {
    _color = value;
    scene()->shaderData.setData(_colorProperty, value);
}

Fog::Fog(Entity* entity) : Component(entity) { setColor(Color(1, 0, 0, 1)); }

void Fog::_onEnable() { scene()->shaderData.addDefine("HAS_FOG"); }

void Fog::_onDisable() { scene()->shaderData.removeDefine("HAS_FOG"); }

}  // namespace vox