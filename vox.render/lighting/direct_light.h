//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/lighting/light.h"

namespace vox {
/**
 * Directional light.
 */
class DirectLight : public Light {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct DirectLightData {
        Vector3F color;
        float _colorPad;  // for align
        Vector3F direction;
        float _directionPad;  // for align
    };

    explicit DirectLight(Entity *entity);

public:
    Vector3F direction();

    Vector3F reverseDirection();

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
    friend class LightManager;

    /**
     * Mount to the current Scene.
     */
    void _onEnable() override;

    /**
     * Unmount from the current Scene.
     */
    void _onDisable() override;

    void _updateShaderData(DirectLightData &shaderData);
};

}  // namespace vox