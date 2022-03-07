//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "point_light.h"
#include "matrix_utils.h"
#include "entity.h"
#include "light_manager.h"

namespace vox {
PointLight::PointLight(Entity *entity) :
Light(entity) {
}

void PointLight::_onEnable() {
    LightManager::getSingleton().attachPointLight(this);
}

void PointLight::_onDisable() {
    LightManager::getSingleton().detachPointLight(this);
}

void PointLight::_updateShaderData(PointLightData &shaderData) {
    shaderData.color = Vector3F(color.r * intensity, color.g * intensity, color.b * intensity);
    auto position = entity()->transform->worldPosition();
    shaderData.position = Vector3F(position.x, position.y, position.z);
    shaderData.distance = distance;
}

//MARK: - Shadow
Matrix4x4F PointLight::shadowProjectionMatrix() {
    return makepPerspective<float>(degreesToRadians(120), 1, 0.1, 100);
}

}
