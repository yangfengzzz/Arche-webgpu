//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting/spot_light.h"

#include "vox.geometry/matrix_utils.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/light_manager.h"

namespace vox {
std::string SpotLight::name() { return "SpotLight"; }

SpotLight::SpotLight(Entity *entity) : Light(entity) {}

void SpotLight::_onEnable() { LightManager::getSingleton().attachSpotLight(this); }

void SpotLight::_onDisable() { LightManager::getSingleton().detachSpotLight(this); }

void SpotLight::_updateShaderData(SpotLightData &shaderData) {
    shaderData.color = Vector3F(color.r * intensity, color.g * intensity, color.b * intensity);
    auto position = entity()->transform->worldPosition();
    shaderData.position = Vector3F(position.x, position.y, position.z);
    auto direction = entity()->transform->worldForward();
    shaderData.direction = Vector3F(direction.x, direction.y, direction.z);
    shaderData.distance = distance;
    shaderData.angleCos = std::cos(angle);
    shaderData.penumbraCos = std::cos(angle + penumbra);
}

// MARK: - Shadow
Matrix4x4F SpotLight::shadowProjectionMatrix() {
    const auto fov = std::min<float>(M_PI / 2.f, angle * 2.f * std::sqrt(2.f));
    return makePerspective<float>(fov, 1, 0.1, distance + 5);
}

// MARK: - Reflection
void SpotLight::onSerialize(nlohmann::json &data) {}

void SpotLight::onDeserialize(nlohmann::json &data) {}

void SpotLight::onInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
