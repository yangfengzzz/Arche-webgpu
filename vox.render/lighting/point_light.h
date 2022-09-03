//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.math/color.h"
#include "vox.render/lighting/light.h"

namespace vox {
/**
 * Point light.
 */
class PointLight : public Light {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct PointLightData {
        Vector3F color;
        float _colorPad;  // for align
        Vector3F position;
        float distance;
    };

    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance = 5;

    explicit PointLight(Entity *entity);

public:
    Matrix4x4F shadowProjectionMatrix() override;

public:
    /**
     * Serialize the component
     */
    void onSerialize(nlohmann::json &data) override;

    /**
     * Deserialize the component
     */
    void onDeserialize(nlohmann::json &data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer &p_root) override;

private:
    /**
     * Mount to the current Scene.
     */
    void _onEnable() override;

    /**
     * Unmount from the current Scene.
     */
    void _onDisable() override;

    void _updateShaderData(PointLightData &shaderData);

private:
    friend class LightManager;
};

}  // namespace vox