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
#include "vox.render/shadow/shadow_material.h"

namespace vox {
class CascadedShadowSubpass : public ForwardSubpass {
public:
    CascadedShadowSubpass(RenderContext* renderContext, Scene* scene, Camera* camera,
                          wgpu::RenderPassDepthStencilAttachment &depthStencilAttachment);

private:
    void _drawElement(wgpu::RenderPassEncoder& passEncoder, const ShaderVariant& variant) override;

    void _renderDirectShadowMap(wgpu::RenderPassEncoder& passEncoder, const ShaderVariant& variant);

    void _updateReceiversShaderData();

    void _getCascadesSplitDistance();

    void _updateShadowSettings();

    void _updateSingleShadowCasterShaderData(DirectLight* light, const ShadowSliceData& shadowSliceData);

    static float _getFarWithRadius(float radius, float denominator);

private:
    friend class ShadowManager;
    wgpu::RenderPassDepthStencilAttachment &_depthStencilAttachment;

    std::vector<RenderElement> opaqueQueue{};
    std::vector<RenderElement> alphaTestQueue{};
    std::vector<RenderElement> transparentQueue{};

    static const std::string _lightViewProjMatProperty;
    static const std::string _lightShadowBiasProperty;
    static const std::string _lightDirectionProperty;

    static const std::string _viewProjMatFromLightProperty;
    static const std::string _shadowInfosProperty;
    static const std::string _shadowSplitSpheresProperty;

    static std::array<float, 5> _cascadesSplitDistance;

    ShadowMode _shadowMode = ShadowMode::SoftLow;
    float _shadowMapResolution{};
    Vector2F _shadowMapSize{};
    float _shadowTileResolution{};
    Vector2F _shadowBias{};
    wgpu::TextureFormat _shadowMapFormat =  wgpu::TextureFormat::Undefined;
    ShadowCascadesMode _shadowCascadeMode = ShadowCascadesMode::FourCascades;
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
    std::array<Vector2F, 4> _viewportOffsets{};

    std::shared_ptr<ShadowMaterial> _shadowMaterial{};
};

}  // namespace vox