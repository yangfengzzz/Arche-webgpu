//
//  direct_light.cpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

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
}

}
