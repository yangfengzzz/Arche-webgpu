//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef point_light_hpp
#define point_light_hpp

#include "light.h"
#include "color.h"

namespace vox {
/**
 * Point light.
 */
class PointLight : public Light {
public:
    struct PointLightData {
        Vector3F color;
        float _colorPad; // for align
        Vector3F position;
        float distance;
    };
    
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance = 5;
    
    PointLight(Entity *entity);
    
public:
    Matrix4x4F shadowProjectionMatrix() override;
    
public:
    /**
     * Serialize the component
     */
    void onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Deserialize the component
     */
    void onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;
    
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

}

#endif /* point_light_hpp */
