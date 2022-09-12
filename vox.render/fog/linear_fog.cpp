//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/fog/linear_fog.h"

#include "vox.render/scene.h"

namespace vox {
const std::string LinearFog::_nearProperty = "u_fogNear";
const std::string LinearFog::_farProperty = "u_fogFar";

float LinearFog::getNear() const { return _near; }

void LinearFog::setNear(float value) {
    _near = value;
    scene()->shaderData.setData(_nearProperty, value);
}

float LinearFog::getFar() const { return _far; }

void LinearFog::setFar(float value) {
    _far = value;
    scene()->shaderData.setData(_farProperty, value);
}

LinearFog::LinearFog(Entity* entity) : Fog(entity) {
    setNear(1);
    setFar(1000);
}

}  // namespace vox