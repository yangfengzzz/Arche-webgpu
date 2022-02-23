//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "direct_light.h"
#include "scene.h"
#include "entity.h"

namespace vox {
DirectLight::DirectLight(Entity *entity) :
Light(entity) {
}

void DirectLight::_onEnable() {
    scene()->light_manager.attachDirectLight(this);
}

void DirectLight::_onDisable() {
    scene()->light_manager.detachDirectLight(this);
}

void DirectLight::_updateShaderData(DirectLightData &shaderData) {
    shaderData.color = Vector3F(color.r * intensity, color.g * intensity, color.b * intensity);
    auto direction = entity()->transform->worldForward();
    shaderData.direction = Vector3F(direction.x, direction.y, direction.z);
}

//MARK: - Shadow
Vector3F DirectLight::direction() {
    return entity()->transform->worldForward();
}

Matrix4x4F DirectLight::shadowProjectionMatrix() {
    assert(false && "cascade shadow don't use this projection");
    throw std::exception();
}

}
