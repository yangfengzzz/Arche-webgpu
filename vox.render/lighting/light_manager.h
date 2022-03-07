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
#include "rendering/compute_pass.h"
#include "singleton.h"

namespace vox {
/**
 * Light Manager.
 */
class LightManager : public Singleton<LightManager> {
public:
    static constexpr uint32_t FORWARD_PLUS_ENABLE = 10;
    static constexpr std::array<uint32_t, 3> TILE_COUNT = {32, 18, 48};
    static constexpr uint32_t TOTAL_TILES = TILE_COUNT[0] * TILE_COUNT[1] * TILE_COUNT[2];
    
    static constexpr std::array<uint32_t, 3> WORKGROUP_SIZE = {4, 2, 4};
    static constexpr std::array<uint32_t, 3> DISPATCH_SIZE = {
        TILE_COUNT[0] / WORKGROUP_SIZE[0],
        TILE_COUNT[1] / WORKGROUP_SIZE[1],
        TILE_COUNT[2] / WORKGROUP_SIZE[2]
    };
    
    // Each cluster tracks up to MAX_LIGHTS_PER_CLUSTER light indices (ints) and one light count.
    // This limitation should be able to go away when we have atomic methods in WGSL.
    static constexpr uint32_t MAX_LIGHTS_PER_CLUSTER = 100;
    static constexpr uint32_t CLUSTER_LIGHTS_SIZE = (8 * TOTAL_TILES) + (4 * MAX_LIGHTS_PER_CLUSTER * TOTAL_TILES) + 4;
    
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
    
private:
    struct ProjectionUniforms {
        Matrix4x4F matrix;
        Matrix4x4F inverseMatrix;
        Vector2F outputSize;
        float zNear;
        float zFar;
    };
    ProjectionUniforms _projectionUniforms;
    ShaderProperty _projectionProp;
    
    struct ViewUniforms {
        Matrix4x4F matrix;
        Vector3F position;
    };
    ViewUniforms _viewUniforms;
    ShaderProperty _viewProp;
    
    struct ClusterBounds {
        Vector3F minAABB;
        Vector3F maxAABB;
    };
    struct Clusters {
        std::array<ClusterBounds, TOTAL_TILES> bounds;
    };
    ShaderProperty _clustersProp;
    std::unique_ptr<Buffer> _clustersBuffer;
    
    struct ClusterLights {
        uint32_t offset;
        uint32_t count;
    };
    struct ClusterLightGroup {
        uint32_t offset;
        std::array<ClusterLights, TOTAL_TILES> lights;
        std::array<uint32_t, MAX_LIGHTS_PER_CLUSTER * TOTAL_TILES> indices;
    };
    ShaderProperty _clusterLightsProp;
    std::unique_ptr<Buffer> _clusterLightsBuffer;
    
    ShaderData _shaderData;
    std::unique_ptr<ComputePass> _clusterBoundsCompute{nullptr};
    std::unique_ptr<ComputePass> _clusterLightsCompute{nullptr};
};

template<> inline LightManager* Singleton<LightManager>::msSingleton{nullptr};

}

#endif /* light_manager_hpp */
