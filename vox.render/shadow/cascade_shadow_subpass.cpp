//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/cascade_shadow_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/components_manager.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/material/material.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/shadow/shadow_manager.h"

namespace vox {
const std::string CascadedShadowSubpass::_lightViewProjMatProperty = "u_lightViewProjMat";
const std::string CascadedShadowSubpass::_lightShadowBiasProperty = "u_shadowBias";
const std::string CascadedShadowSubpass::_lightDirectionProperty = "u_lightDirection";

const std::string CascadedShadowSubpass::_viewProjMatFromLightProperty = "u_viewProjMatFromLight";
const std::string CascadedShadowSubpass::_shadowInfosProperty = "u_shadowInfo";
const std::string CascadedShadowSubpass::_shadowMapsProperty = "u_shadowMap";
const std::string CascadedShadowSubpass::_shadowSplitSpheresProperty = "u_shadowSplitSpheres";

std::array<float, 5> CascadedShadowSubpass::_cascadesSplitDistance{};

CascadedShadowSubpass::CascadedShadowSubpass(RenderContext* renderContext,
                                             wgpu::TextureFormat depthStencilTextureFormat,
                                             Scene* scene,
                                             Camera* camera)
    : ForwardSubpass(renderContext, depthStencilTextureFormat, scene, camera) {
    fragmentEnabled = false;
}

void CascadedShadowSubpass::_drawElement(wgpu::RenderPassEncoder& passEncoder, const ShaderVariant& variant) {
    std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
    std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);
    std::sort(transparentQueue.begin(), transparentQueue.end(), _compareFromFarToNear);

    for (const auto& element : opaqueQueue) {
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
    for (const auto& element : alphaTestQueue) {
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
    for (const auto& element : transparentQueue) {
        ForwardSubpass::_drawElement(passEncoder, element, variant);
    }
}

void CascadedShadowSubpass::_renderDirectShadowMap() {}

void CascadedShadowSubpass::_updateReceiversShaderData() {
    for (uint32_t i = shadowCascades * 4, n = 4 * 4; i < n; i++) {
        _splitBoundSpheres[i] = 0.0;
    }

    auto& shaderData = _scene->shaderData;
    shaderData.setData(CascadedShadowSubpass::_viewProjMatFromLightProperty, _vpMatrix);
    shaderData.setData(CascadedShadowSubpass::_shadowInfosProperty, _shadowInfos);
    shaderData.setData(CascadedShadowSubpass::_shadowSplitSpheresProperty, _splitBoundSpheres);

    shaderData.setData(CascadedShadowSubpass::_shadowMapsProperty, _depthTexture);  // todo
}

void CascadedShadowSubpass::_getCascadesSplitDistance() {
    float nearClipPlane = _camera->nearClipPlane();
    float farClipPlane = _camera->farClipPlane();
    float aspectRatio = _camera->aspectRatio();
    float fieldOfView = _camera->fieldOfView();

    CascadedShadowSubpass::_cascadesSplitDistance[0] = nearClipPlane;
    float range = farClipPlane - nearClipPlane;
    float tFov = std::tan(degreesToRadians(fieldOfView) * 0.5f);
    float denominator = 1.f + tFov * tFov * (aspectRatio * aspectRatio + 1.f);
    switch (shadowCascades) {
        case ShadowCascadesMode::NoCascades:
            CascadedShadowSubpass::_cascadesSplitDistance[1] = _getFarWithRadius(farClipPlane, denominator);
            break;

        case ShadowCascadesMode::TwoCascades:
            CascadedShadowSubpass::_cascadesSplitDistance[1] =
                    _getFarWithRadius(nearClipPlane + range * shadowTwoCascadeSplits, denominator);
            CascadedShadowSubpass::_cascadesSplitDistance[2] = _getFarWithRadius(farClipPlane, denominator);
            break;

        case ShadowCascadesMode::FourCascades:
            CascadedShadowSubpass::_cascadesSplitDistance[1] =
                    _getFarWithRadius(nearClipPlane + range * shadowFourCascadeSplits.x, denominator);
            CascadedShadowSubpass::_cascadesSplitDistance[2] =
                    _getFarWithRadius(nearClipPlane + range * shadowFourCascadeSplits.y, denominator);
            CascadedShadowSubpass::_cascadesSplitDistance[3] =
                    _getFarWithRadius(nearClipPlane + range * shadowFourCascadeSplits.z, denominator);
            CascadedShadowSubpass::_cascadesSplitDistance[4] = _getFarWithRadius(farClipPlane, denominator);
            break;
    }
}

float CascadedShadowSubpass::_getFarWithRadius(float radius, float denominator) {
    // use the frustum side as the radius and get the far distance form camera.
    // float tFov = std::tan(fov * 0.5);// get this the equation using Pythagorean
    // return std::sqrt(radius * radius / (1.0 + tFov * tFov * (aspectRatio * aspectRatio + 1.0)));
    return std::sqrt((radius * radius) / denominator);
}

void CascadedShadowSubpass::_updateShadowSettings() {}

void CascadedShadowSubpass::_updateSingleShadowCasterShaderData(DirectLight* light,
                                                                const ShadowSliceData& shadowSliceData) {
    // Frustum size is guaranteed to be a cube as we wrap shadow frustum around a sphere
    // elements[0] = 2.0 / (right - left)
    float frustumSize = 2.f / shadowSliceData.projectionMatrix(0, 0);
    // depth and normal bias scale is in shadowMap texel size in world space
    float texelSize = frustumSize / _shadowTileResolution;
    _shadowBias.set(-light->shadowBias * texelSize, -light->shadowNormalBias * texelSize);

    auto& sceneShaderData = _scene->shaderData;
    sceneShaderData.setData(CascadedShadowSubpass::_lightShadowBiasProperty, _shadowBias);
    sceneShaderData.setData(CascadedShadowSubpass::_lightDirectionProperty, light->direction());
    sceneShaderData.setData(CascadedShadowSubpass::_lightViewProjMatProperty, shadowSliceData.viewProjectMatrix);
}

}  // namespace vox
