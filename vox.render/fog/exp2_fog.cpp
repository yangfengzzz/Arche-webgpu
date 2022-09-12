//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/fog/exp2_fog.h"

#include "vox.render/scene.h"

namespace vox {
const std::string Exp2Fog::_densityProperty = "u_fogDensity";

float Exp2Fog::getDensity() const { return _density; }

void Exp2Fog::setDensity(float value) {
    _density = value;
    scene()->shaderData.setData(_densityProperty, value);
}

Exp2Fog::Exp2Fog(Entity* entity) : Fog(entity) { setDensity(0.0025); }

void Exp2Fog::_onEnable() {
    Fog::_onEnable();
    scene()->shaderData.addDefine("FOG_EXP2");
}

void Exp2Fog::_onDisable() {
    Fog::_onDisable();
    scene()->shaderData.removeDefine("FOG_EXP2");
}

}  // namespace vox