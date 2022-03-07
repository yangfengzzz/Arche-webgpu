//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef light_manager_hpp
#define light_manager_hpp

#include "point_light.h"
#include "spot_light.h"
#include "direct_light.h"
#include "shader/shader_data.h"
#include "singleton.h"

namespace vox {
/**
 * Light Manager.
 */
class LightManager : public Singleton<LightManager> {
public:
    static LightManager &getSingleton(void);
    
    static LightManager *getSingletonPtr(void);
    
    LightManager(Scene* scene);
        
    void setCamera(Camera* camera);
    
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attachPointLight(PointLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detachPointLight(PointLight *light);
    
    const std::vector<PointLight *> &pointLights() const;
    
public:
    /**
     * Register a light object to the current scene.
     * @param light render light
     */
    void attachSpotLight(SpotLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light render light
     */
    void detachSpotLight(SpotLight *light);
    
    const std::vector<SpotLight *> &spotLights() const;
    
public:
    /**
     * Register a light object to the current scene.
     * @param light direct light
     */
    void attachDirectLight(DirectLight *light);
    
    /**
     * Remove a light object from the current scene.
     * @param light direct light
     */
    void detachDirectLight(DirectLight *light);
    
    const std::vector<DirectLight *> &directLights() const;
    
public:
    void draw(wgpu::CommandEncoder& commandEncoder);
        
private:
    Scene* _scene{nullptr};
    Camera* _camera{nullptr};
    
    std::vector<PointLight *> _pointLights;
    std::vector<PointLight::PointLightData> _pointLightDatas;
    ShaderProperty _pointLightProperty;
    
    std::vector<SpotLight *> _spotLights;
    std::vector<SpotLight::SpotLightData> _spotLightDatas;
    ShaderProperty _spotLightProperty;
    
    std::vector<DirectLight *> _directLights;
    std::vector<DirectLight::DirectLightData> _directLightDatas;
    ShaderProperty _directLightProperty;
    
    void _updateShaderData(ShaderData &shaderData);
};

template<> inline LightManager* Singleton<LightManager>::msSingleton{nullptr};

}

#endif /* light_manager_hpp */
