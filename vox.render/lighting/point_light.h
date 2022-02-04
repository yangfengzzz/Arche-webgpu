//
//  point_light.hpp
//  vox.render
//
//  Created by 杨丰 on 2022/1/19.
//

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
        float _colorAlign;
        Vector3F position;
        float _positionAlign;
        float distance;
    };
    
    /** Light color. */
    Color color = Color(1, 1, 1, 1);
    /** Light intensity. */
    float intensity = 1.0;
    /** Defines a distance cutoff at which the light's intensity must be considered zero. */
    float distance = 100;
    
    PointLight(Entity *entity);
    
public:
    Matrix4x4F shadowProjectionMatrix() override;
    
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
