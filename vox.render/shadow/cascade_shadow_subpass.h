//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/rendering/subpasses/forward_subpass.h"
#include "vox.render/shadow/enum/shadow_cascades_mode.h"
#include "vox.render/shadow/enum/shadow_mode.h"
#include "vox.render/shadow/enum/shadow_resolution.h"
#include "vox.render/shadow/shadow_slice_data.h"

namespace vox {
class CascadedShadowSubpass : public ForwardSubpass {
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

    CascadedShadowSubpass(RenderContext* renderContext,
                          wgpu::TextureFormat depthStencilTextureFormat,
                          Scene* scene,
                          Camera* camera);

    void _drawElement(wgpu::RenderPassEncoder& passEncoder, const ShaderVariant& variant) override;

private:
    void _renderDirectShadowMap(wgpu::RenderPassEncoder& passEncoder, const ShaderVariant& variant);

    void _updateReceiversShaderData();

    void _getCascadesSplitDistance();

    static float _getFarWithRadius(float radius, float denominator);

    void _updateShadowSettings();

    void _updateSingleShadowCasterShaderData(DirectLight* light, const ShadowSliceData& shadowSliceData);

private:
    std::vector<RenderElement> opaqueQueue;
    std::vector<RenderElement> alphaTestQueue;
    std::vector<RenderElement> transparentQueue;

    static const std::string _lightViewProjMatProperty;
    static const std::string _lightShadowBiasProperty;
    static const std::string _lightDirectionProperty;

    static const std::string _viewProjMatFromLightProperty;
    static const std::string _shadowInfosProperty;
    static const std::string _shadowMapsProperty;
    static const std::string _shadowSplitSpheresProperty;

    static std::array<float, 5> _cascadesSplitDistance;

    ShadowMode _shadowMode;
    float _shadowMapResolution{};
    Vector2F _shadowMapSize{};
    float _shadowTileResolution{};
    Vector2F _shadowBias;
    wgpu::TextureFormat _shadowMapFormat;
    ShadowCascadesMode _shadowCascadeMode;
    ShadowSliceData _shadowSliceData{};
    Vector3F _lightUp{};
    Vector3F _lightSide{};
    Vector3F _lightForward{};
    bool _existShadowMap = false;

    std::array<float, 16> _splitBoundSpheres{};
    // 4 viewProj matrix for cascade shadow
    std::array<Matrix4x4F, 4> _vpMatrix{};
    // strength, resolution, lightIndex
    Vector3F _shadowInfos{};
    wgpu::Texture _depthTexture;
    std::array<Vector2F, 4> _viewportOffsets{};
};

}  // namespace vox