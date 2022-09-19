//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/lighting/point_light.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/shadow/cascade_shadow_subpass.h"
#include "vox.render/singleton.h"

namespace vox {
class ShadowManager : public Singleton<ShadowManager> {
public:
    /** How this light casts shadows */
    ShadowMode shadowMode = ShadowMode::SoftLow;
    /** The resolution of the shadow maps. */
    ShadowResolution shadowResolution = ShadowResolution::High;
    /** Number of cascades to use for directional light shadows. */
    ShadowCascadesMode shadowCascades = ShadowCascadesMode::FourCascades;
    /** The splits of two cascade distribution. */
    float shadowTwoCascadeSplits = 1.0 / 3.0;
    /** The splits of four cascade distribution. */
    Vector3F shadowFourCascadeSplits = Vector3F(1.0 / 15, 3.0 / 15.0, 7.0 / 15.0);

    static ShadowManager& getSingleton();

    static ShadowManager* getSingletonPtr();

public:
    ShadowManager(Scene* scene, Camera* camera);

    void draw(wgpu::CommandEncoder& commandEncoder);

private:
    void _drawSpotShadowMap(wgpu::CommandEncoder& commandEncoder);

    void _drawDirectShadowMap(wgpu::CommandEncoder& commandEncoder);

    void _drawPointShadowMap(wgpu::CommandEncoder& commandEncoder);

    void _getAvailableRenderTarget();

private:
    Scene* _scene{nullptr};
    Camera* _camera{nullptr};

    wgpu::RenderPassDescriptor _renderPassDescriptor{};
    wgpu::RenderPassDepthStencilAttachment _depthStencilAttachment{};

    std::unique_ptr<RenderPass> _renderPass{nullptr};
    CascadedShadowSubpass* _cascadedShadowSubpass{nullptr};

    wgpu::SamplerDescriptor _samplerDescriptor{};
    std::unique_ptr<Image> _depthTexture{};
    static const std::string _shadowTextureProperty;
    static const std::string _shadowSamplerProperty;
};

template <>
inline ShadowManager* Singleton<ShadowManager>::ms_singleton{nullptr};

}  // namespace vox