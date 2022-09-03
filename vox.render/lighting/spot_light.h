//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef spot_light_hpp
#define spot_light_hpp

#include "light.h"
#include "vox.math/color.h"

namespace vox {
/**
 * Spot light.
 */
class SpotLight : public Light {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct SpotLightData {
        Vector3F color;
        float _colorPad;  // for align
        Vector3F position;
        float _positionPad;  // for align
        Vector3F direction;
        float distance;
        float angleCos;
        float penumbraCos;
        float _pad;   // for align
        float _pad2;  // for align
    };

    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance = 5;
    /** Angle, in radians, from centre of spotlight where falloff begins. */
    float angle = M_PI / 6;
    /** Angle, in radians, from falloff begins to ends. */
    float penumbra = M_PI / 12;

    SpotLight(Entity *entity);

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
    friend class LightManager;

    /**
     * Mount to the current Scene.
     */
    void _onEnable() override;

    /**
     * Unmount from the current Scene.
     */
    void _onDisable() override;

    void _updateShaderData(SpotLightData &shaderData);
};

}  // namespace vox

#endif /* spot_light_hpp */
