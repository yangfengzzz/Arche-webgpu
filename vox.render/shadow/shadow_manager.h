//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shadow_manager_hpp
#define shadow_manager_hpp

#include "rendering/render_pass.h"
#include "shadow_subpass.h"
#include "scene.h"
#include "texture/sampled_texture2d.h"
#include "texture/sampled_texturecube.h"

namespace vox {
class ShadowManager {
public:
    static constexpr uint32_t SHADOW_MAP_CASCADE_COUNT = 4;
    static constexpr uint32_t MAX_SHADOW = 10;
    static constexpr uint32_t MAX_CUBE_SHADOW = 5;
    static constexpr uint32_t SHADOW_MAP_RESOLUTION = 4000;
    static constexpr wgpu::TextureFormat SHADOW_MAP_FORMAT = wgpu::TextureFormat::Depth32Float;

    struct ShadowData {
        /**
         * Shadow bias.
         */
        float bias = 0.005;
        
        /**
         * Shadow intensity, the larger the value, the clearer and darker the shadow.
         */
        float intensity = 0.2;
        
        /**
         * Pixel range used for shadow PCF interpolation.
         */
        float radius = 1;
        
        /** Alignment */
        float dump = 0;
        
        /**
         * Light view projection matrix.(cascade)
         */
        Matrix4x4F vp[4];
        
        /**
         * Light cascade depth.
         */
        float cascadeSplits[4];
    };
    
    struct CubeShadowData {
        /**
         * Shadow bias.
         */
        float bias = 0.005;
        
        /**
         * Shadow intensity, the larger the value, the clearer and darker the shadow.
         */
        float intensity = 0.2;
        
        /**
         * Pixel range used for shadow PCF interpolation.
         */
        float radius = 1;
        
        /** Alignment */
        float dump = 0;
        
        /**
         * Light view projection matrix.(cascade)
         */
        Matrix4x4F vp[6];
        
        Vector4F lightPos;
    };
    
public:
    ShadowManager(Scene* scene, Camera* camera);
    
    float cascadeSplitLambda();
    
    void setCascadeSplitLambda(float value);
    
    void draw(wgpu::CommandEncoder& commandEncoder);
    
private:
    void _drawSpotShadowMap(wgpu::CommandEncoder& commandEncoder);
    
    void _drawDirectShadowMap(wgpu::CommandEncoder& commandEncoder);
    
    void _drawPointShadowMap(wgpu::CommandEncoder& commandEncoder);
    
    void _updateSpotShadow(SpotLight* light, ShadowManager::ShadowData& shadowData);
    
    /*
     * Calculate frustum split depths and matrices for the shadow map cascades
     * Based on https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
     */
    void _updateCascadesShadow(DirectLight *light, ShadowManager::ShadowData& shadowData);
    
    void _updatePointShadow(PointLight *light, ShadowManager::CubeShadowData& shadowData);
    
private:
    Scene* _scene{nullptr};
    Camera* _camera{nullptr};
    
    wgpu::RenderPassDescriptor _renderPassDescriptor;
    wgpu::RenderPassDepthStencilAttachment _depthStencilAttachment;

    std::unique_ptr<RenderPass> _renderPass{nullptr};
    ShadowSubpass* _shadowSubpass{nullptr};
    
    float _cascadeSplitLambda = 0.5f;
    
    uint32_t _cubeShadowCount = 0;
    std::array<wgpu::Texture, 6> _cubeMapSlices{};
    std::vector<wgpu::Texture> _cubeShadowMaps{};
    uint32_t _shadowCount = 0;
    std::array<wgpu::Texture, SHADOW_MAP_CASCADE_COUNT> _cascadeShadowMaps{};
    std::vector<wgpu::Texture> _shadowMaps{};
    
    ShaderProperty _shadowCountProp;
    ShaderProperty _shadowMapProp;
    ShaderProperty _shadowSamplerProp;
    ShaderProperty _shadowDataProp;
    SampledTexture2DPtr _packedTexture{nullptr};
    std::array<ShadowManager::ShadowData, ShadowManager::MAX_SHADOW> _shadowDatas{};
    
    ShaderProperty _cubeShadowCountProp;
    ShaderProperty _cubeShadowMapProp;
    ShaderProperty _cubeShadowSamplerProp;
    ShaderProperty _cubeShadowDataProp;
    SampledTextureCubePtr _packedCubeTexture{nullptr};
    std::array<ShadowManager::CubeShadowData, ShadowManager::MAX_CUBE_SHADOW> _cubeShadowDatas{};
    
    const std::array<std::pair<Vector3F, Vector3F>, 6> _cubeMapDirection = {
        std::make_pair(Vector3F(10, 0, 0), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(-10, 0, 0), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(0, 10, 0), Vector3F(1, 0, 0)),
        std::make_pair(Vector3F(0, -10, 0), Vector3F(1, 0, 0)),
        std::make_pair(Vector3F(0, 0, 10), Vector3F(0, 1, 0)),
        std::make_pair(Vector3F(0, 0, -10), Vector3F(0, 1, 0)),
    };
};
}
#endif /* shadow_manager_hpp */
