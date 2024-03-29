//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/shadow/cascade_shadow_subpass.h"

#include "vox.render/camera.h"
#include "vox.render/components_manager.h"
#include "vox.render/entity.h"
#include "vox.render/lighting/direct_light.h"
#include "vox.render/lighting/light_manager.h"
#include "vox.render/mesh/mesh.h"
#include "vox.render/renderer.h"
#include "vox.render/rendering/render_pass.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shadow/shadow_manager.h"
#include "vox.render/shadow/shadow_utils.h"

namespace vox {
const std::string CascadedShadowSubpass::_lightViewProjMatProperty = "u_lightViewProjMat";
const std::string CascadedShadowSubpass::_lightShadowBiasProperty = "u_shadowBias";
const std::string CascadedShadowSubpass::_lightDirectionProperty = "u_lightDirection";

const std::string CascadedShadowSubpass::_viewProjMatFromLightProperty = "u_viewProjMatFromLight";
const std::string CascadedShadowSubpass::_shadowInfosProperty = "u_shadowInfo";
const std::string CascadedShadowSubpass::_shadowSplitSpheresProperty = "u_shadowSplitSpheres";

std::array<float, 5> CascadedShadowSubpass::_cascadesSplitDistance{};

CascadedShadowSubpass::CascadedShadowSubpass(RenderContext* renderContext,
                                             Scene* scene,
                                             Camera* camera,
                                             wgpu::RenderPassDepthStencilAttachment& depthStencilAttachment)
    : ForwardSubpass(renderContext, wgpu::TextureFormat::Depth16Unorm, scene, camera),
      _bufferPool(scene->device(), 256 * 4, wgpu::BufferUsage::Uniform | wgpu::BufferUsage::CopyDst),
      _depthStencilAttachment(depthStencilAttachment) {
    _shadowMaterial = std::make_shared<ShadowMaterial>(scene->device());
}

void CascadedShadowSubpass::_drawElement(wgpu::RenderPassEncoder& passEncoder, ShaderVariant& variant) {
    _bufferPool.reset();
    _existShadowMap = false;
    _renderDirectShadowMap(passEncoder, variant);

    if (_existShadowMap) {
        _updateReceiversShaderData();
        _scene->shaderData.addDefine(CASCADED_SHADOW_MAP);
    } else {
        _scene->shaderData.removeDefine(CASCADED_SHADOW_MAP);
    }
}

void CascadedShadowSubpass::_renderDirectShadowMap(wgpu::RenderPassEncoder& passEncoder, const ShaderVariant& variant) {
    ShadowCascadesMode shadowCascades = ShadowManager::GetSingleton().shadowCascades;
    const auto& lights = LightManager::GetSingleton().directLights();
    const uint32_t sunLightIndex = LightManager::GetSingleton().getSunLightIndex();
    const auto& boundSphere = _shadowSliceData.splitBoundSphere;
    auto& bufferBlock = _bufferPool.requestBufferBlock(4 * sizeof(Matrix4x4F));

    _getCascadesSplitDistance();
    if (sunLightIndex != -1) {
        const auto& light = lights[sunLightIndex];
        if (light->enableShadow) {
            _shadowInfos.x = light->shadowStrength;
            _shadowInfos.y = _shadowTileResolution;
            _shadowInfos.z = float(sunLightIndex);

            // prepare light and camera direction
            Matrix4x4F lightWorld = light->entity()->transform->worldRotationQuaternion().matrix4();
            _lightSide.set(lightWorld[0], lightWorld[1], lightWorld[2]);
            _lightUp.set(lightWorld[4], lightWorld[5], lightWorld[6]);
            _lightForward.set(-lightWorld[8], -lightWorld[9], -lightWorld[10]);
            auto cameraWorldForward = _camera->entity()->transform->worldForward();

            for (uint32_t j = 0; j < shadowCascades; j++) {
                if (j == 0) {
                    _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
                } else {
                    _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Load;
                }
                passEncoder.SetViewport(_viewportOffsets[j].x, _viewportOffsets[j].y, _shadowTileResolution,
                                        _shadowTileResolution, 0, 1);

                ShadowUtils::getBoundSphereByFrustum(_cascadesSplitDistance[j], _cascadesSplitDistance[j + 1], _camera,
                                                     cameraWorldForward.normalized(), _shadowSliceData);
                ShadowUtils::getDirectionLightShadowCullPlanes(_camera->frustum(), _cascadesSplitDistance[j],
                                                               _camera->nearClipPlane(), _lightForward,
                                                               _shadowSliceData);
                ShadowUtils::getDirectionalLightMatrices(_lightUp, _lightSide, _lightForward, light->shadowNearPlane,
                                                         _shadowTileResolution, _shadowSliceData);
                _updateSingleShadowCasterShaderData(bufferBlock, light, _shadowSliceData);

                // upload pre-cascade infos.
                const auto& center = boundSphere.center;
                const auto& radius = boundSphere.radius;
                const uint32_t offset = j * 4;
                _splitBoundSpheres[offset] = center.x;
                _splitBoundSpheres[offset + 1] = center.y;
                _splitBoundSpheres[offset + 2] = center.z;
                _splitBoundSpheres[offset + 3] = radius * radius;
                _vpMatrix[j] = _shadowSliceData.viewProjectMatrix;

                opaqueQueue.clear();
                alphaTestQueue.clear();
                transparentQueue.clear();
                const auto& renderers = ComponentsManager::GetSingleton()._renderers;
                for (auto renderer : renderers) {
                    ShadowUtils::shadowCullFrustum(renderer, _shadowSliceData, opaqueQueue, alphaTestQueue,
                                                   transparentQueue);
                }
                std::sort(opaqueQueue.begin(), opaqueQueue.end(), _compareFromNearToFar);
                std::sort(alphaTestQueue.begin(), alphaTestQueue.end(), _compareFromNearToFar);

                for (auto& element : opaqueQueue) {
                    element.material = _shadowMaterial;
                    ForwardSubpass::_drawElement(passEncoder, element, variant);
                }
                for (auto& element : alphaTestQueue) {
                    element.material = _shadowMaterial;
                    ForwardSubpass::_drawElement(passEncoder, element, variant);
                }
            }
            _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
            _existShadowMap = true;
        }
    }
}

void CascadedShadowSubpass::_updateReceiversShaderData() {
    ShadowCascadesMode shadowCascades = ShadowManager::GetSingleton().shadowCascades;
    for (uint32_t i = shadowCascades * 4, n = 4 * 4; i < n; i++) {
        _splitBoundSpheres[i] = 0.0;
    }

    auto& shaderData = _scene->shaderData;
    shaderData.setData(CascadedShadowSubpass::_viewProjMatFromLightProperty, _vpMatrix);
    shaderData.setData(CascadedShadowSubpass::_shadowInfosProperty, _shadowInfos);
    shaderData.setData(CascadedShadowSubpass::_shadowSplitSpheresProperty, _splitBoundSpheres);
}

void CascadedShadowSubpass::_getCascadesSplitDistance() {
    ShadowCascadesMode shadowCascades = ShadowManager::GetSingleton().shadowCascades;
    float shadowTwoCascadeSplits = ShadowManager::GetSingleton().shadowTwoCascadeSplits;
    Vector3F shadowFourCascadeSplits = ShadowManager::GetSingleton().shadowFourCascadeSplits;
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

void CascadedShadowSubpass::_updateShadowSettings() {
    ShadowCascadesMode shadowCascades = ShadowManager::GetSingleton().shadowCascades;
    ShadowResolution shadowResolution = ShadowManager::GetSingleton().shadowResolution;
    ShadowMode shadowMode = ShadowManager::GetSingleton().shadowMode;

    auto& sceneShaderData = _scene->shaderData;
    const wgpu::TextureFormat shadowFormat = ShadowUtils::shadowDepthFormat(shadowResolution);
    const float resolution = ShadowUtils::shadowResolution(shadowResolution);
    if (shadowCascades != _shadowCascadeMode) {
        sceneShaderData.addDefine(CASCADED_COUNT + std::to_string(shadowCascades));
    }
    if (shadowMode != _shadowMode) {
        sceneShaderData.addDefine(SHADOW_MODE + std::to_string(shadowMode));
        _shadowMode = shadowMode;
    }

    if (shadowFormat != _shadowMapFormat || resolution != _shadowMapResolution ||
        shadowCascades != _shadowCascadeMode) {
        _shadowMapFormat = shadowFormat;
        _shadowMapResolution = resolution;
        _shadowCascadeMode = shadowCascades;
        _depthStencil.format = _shadowMapFormat;

        if (shadowCascades == ShadowCascadesMode::NoCascades) {
            _shadowTileResolution = resolution;
            _shadowMapSize.set(resolution, resolution);
        } else {
            const float shadowTileResolution =
                    ShadowUtils::getMaxTileResolutionInAtlas(resolution, resolution, shadowCascades);
            _shadowTileResolution = shadowTileResolution;
            _shadowMapSize.set(shadowTileResolution * 2, shadowCascades == ShadowCascadesMode::TwoCascades
                                                                 ? shadowTileResolution
                                                                 : shadowTileResolution * 2);
        }

        switch (shadowCascades) {
            case ShadowCascadesMode::NoCascades:
                _viewportOffsets[0].set(0, 0);
                break;
            case ShadowCascadesMode::TwoCascades:
                _viewportOffsets[0].set(0, 0);
                _viewportOffsets[1].set(_shadowTileResolution, 0);
                break;
            case ShadowCascadesMode::FourCascades:
                _viewportOffsets[0].set(0, 0);
                _viewportOffsets[1].set(_shadowTileResolution, 0);
                _viewportOffsets[2].set(0, _shadowTileResolution);
                _viewportOffsets[3].set(_shadowTileResolution, _shadowTileResolution);
        }
    }
}

void CascadedShadowSubpass::_updateSingleShadowCasterShaderData(BufferBlock& bufferBlock,
                                                                DirectLight* light,
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

    auto allocation = bufferBlock.allocate(sizeof(Matrix4x4F));
    allocation.update(_scene->device(), shadowSliceData.viewProjectMatrix);
    sceneShaderData.setData(CascadedShadowSubpass::_lightViewProjMatProperty, std::move(allocation));
}

}  // namespace vox
