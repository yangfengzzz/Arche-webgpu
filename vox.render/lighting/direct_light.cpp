//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/direct_light.h"

#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"

namespace vox {
std::string DirectLight::name() { return "DirectLight"; }

DirectLight::DirectLight(Entity *entity) : Light(entity) {}

void DirectLight::_onEnable() { LightManager::getSingleton().attachDirectLight(this); }

void DirectLight::_onDisable() { LightManager::getSingleton().detachDirectLight(this); }

void DirectLight::_updateShaderData(DirectLightData &shaderData) {
    shaderData.color = Vector3F(color.r * intensity, color.g * intensity, color.b * intensity);
    auto direction = entity()->transform->worldForward();
    shaderData.direction = Vector3F(direction.x, direction.y, direction.z);
}

// MARK: - Shadow
Vector3F DirectLight::direction() { return entity()->transform->worldForward(); }

Vector3F DirectLight::reverseDirection() { return -entity()->transform->worldForward(); }

// MARK: - Reflection
void DirectLight::onSerialize(nlohmann::json &data) {}

void DirectLight::onDeserialize(nlohmann::json &data) {}

void DirectLight::onInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
