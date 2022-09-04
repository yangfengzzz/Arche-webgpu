//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/lighting/direct_light.h"
#include "vox.render/lighting/point_light.h"
#include "vox.render/lighting/spot_light.h"
#include "vox.render/rendering/compute_pass.h"
#include "vox.render/shader/shader_data.h"
#include "vox.render/singleton.h"

namespace vox {
/**
 * Light Manager.
 */
class LightManager : public Singleton<LightManager> {
public:
    static constexpr uint32_t FORWARD_PLUS_ENABLE_MIN_COUNT = 20;
    static constexpr std::array<uint32_t, 3> TILE_COUNT = {32, 18, 48};
    static constexpr uint32_t TOTAL_TILES = TILE_COUNT[0] * TILE_COUNT[1] * TILE_COUNT[2];

    static constexpr std::array<uint32_t, 3> WORKGROUP_SIZE = {4, 2, 4};
    static constexpr std::array<uint32_t, 3> DISPATCH_SIZE = {
            TILE_COUNT[0] / WORKGROUP_SIZE[0], TILE_COUNT[1] / WORKGROUP_SIZE[1], TILE_COUNT[2] / WORKGROUP_SIZE[2]};

    // Each cluster tracks up to MAX_LIGHTS_PER_CLUSTER light indices (ints) and one light count.
    // This limitation should be able to go away when we have atomic methods in WGSL.
    static constexpr uint32_t MAX_LIGHTS_PER_CLUSTER = 50;

    static LightManager &getSingleton();

    static LightManager *getSingletonPtr();

    explicit LightManager(Scene *scene);

    void setCamera(Camera *camera);

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

    [[nodiscard]] const std::vector<PointLight *> &pointLights() const;

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

    [[nodiscard]] const std::vector<SpotLight *> &spotLights() const;

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

    [[nodiscard]] const std::vector<DirectLight *> &directLights() const;

public:
    [[nodiscard]] bool enableForwardPlus() const;

    void draw(wgpu::CommandEncoder &commandEncoder);

private:
    Scene *_scene{nullptr};
    Camera *_camera{nullptr};

    std::vector<PointLight *> _pointLights;
    std::vector<PointLight::PointLightData> _pointLightDatas;
    const std::string _pointLightProperty;

    std::vector<SpotLight *> _spotLights;
    std::vector<SpotLight::SpotLightData> _spotLightDatas;
    const std::string _spotLightProperty;

    std::vector<DirectLight *> _directLights;
    std::vector<DirectLight::DirectLightData> _directLightDatas;
    const std::string _directLightProperty;

    void _updateShaderData(ShaderData &shaderData);

private:
    bool _enableForwardPlus{false};

    struct ProjectionUniforms {
        Matrix4x4F matrix;
        Matrix4x4F inverseMatrix;
        Vector2F outputSize;
        float zNear;
        float zFar;
        Matrix4x4F viewMatrix;
    };
    ProjectionUniforms _forwardPlusUniforms;
    const std::string _forwardPlusProp;

    struct ClusterBounds {
        Vector3F minAABB;
        float _pad1;
        Vector3F maxAABB;
        float _pad2;
    };
    struct Clusters {
        std::array<ClusterBounds, TOTAL_TILES> bounds;
    };
    const std::string _clustersProp;
    std::unique_ptr<Buffer> _clustersBuffer;

    struct ClusterLights {
        uint32_t offset;
        uint32_t point_count;
        uint32_t spot_count;
    };
    struct ClusterLightGroup {
        uint32_t offset;
        std::array<ClusterLights, TOTAL_TILES> lights;
        std::array<uint32_t, MAX_LIGHTS_PER_CLUSTER * TOTAL_TILES> indices;
    };
    const std::string _clusterLightsProp;
    std::unique_ptr<Buffer> _clusterLightsBuffer;

    ShaderData _shaderData;
    std::unique_ptr<ComputePass> _clusterBoundsCompute{nullptr};
    std::unique_ptr<ComputePass> _clusterLightsCompute{nullptr};
};

template <>
inline LightManager *Singleton<LightManager>::ms_singleton{nullptr};

}  // namespace vox