//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef direct_light_hpp
#define direct_light_hpp

#include "light.h"
#include "color.h"

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
        float _colorPad; // for align
        Vector3F direction;
        float _directionPad; // for align
    };
    
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    
    DirectLight(Entity *entity);
    
public:
    Matrix4x4F shadowProjectionMatrix() override;
    
    Vector3F direction();
    
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

}

#endif /* direct_light_hpp */
