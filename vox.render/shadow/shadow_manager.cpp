//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "shadow_manager.h"
#include "shader/shader.h"
#include "camera.h"
#include "matrix_utils.h"
#include "texture/texture_utils.h"
#include "texture/sampled_texture2d.h"
#include "texture/sampled_texturecube.h"

namespace vox {
ShadowManager::ShadowManager(Scene* scene, Camera* camera):
_scene(scene),
_camera(camera),
_shadowMapProp(Shader::createProperty("u_shadowMap", ShaderDataGroup::Scene)),
_shadowSamplerProp(Shader::createProperty("u_shadowSampler", ShaderDataGroup::Scene)),
_shadowDataProp(Shader::createProperty("u_shadowData", ShaderDataGroup::Scene)),
_shadowCountProp(Shader::createProperty("u_shadowCount", ShaderDataGroup::Scene)),

_cubeShadowMapProp(Shader::createProperty("u_cubeShadowMap", ShaderDataGroup::Scene)),
_cubeShadowSamplerProp(Shader::createProperty("u_cubeShadowSampler", ShaderDataGroup::Scene)),
_cubeShadowDataProp(Shader::createProperty("u_cubeShadowData", ShaderDataGroup::Scene)),
_cubeShadowCountProp(Shader::createProperty("u_cubeShadowCount", ShaderDataGroup::Scene)) {
    _renderPassDescriptor.colorAttachmentCount = 0;
    _renderPassDescriptor.colorAttachments = nullptr;
    _renderPassDescriptor.depthStencilAttachment = &_depthStencilAttachment;
    _depthStencilAttachment.depthLoadOp = wgpu::LoadOp::Clear;
    _depthStencilAttachment.clearDepth = 1.0;
    _depthStencilAttachment.depthStoreOp = wgpu::StoreOp::Store;
    _depthStencilAttachment.stencilLoadOp = wgpu::LoadOp::Load;
    _depthStencilAttachment.stencilStoreOp = wgpu::StoreOp::Discard;

    _renderPass = std::make_unique<RenderPass>(_scene->device(), _renderPassDescriptor);
    auto shadowSubpass = std::make_unique<ShadowSubpass>(nullptr, _scene, _camera);
    _shadowSubpass = shadowSubpass.get();
    _renderPass->addSubpass(std::move(shadowSubpass));
}

float ShadowManager::cascadeSplitLambda() {
    return _cascadeSplitLambda;
}

void ShadowManager::setCascadeSplitLambda(float value) {
    _cascadeSplitLambda = value;
}

void ShadowManager::draw(wgpu::CommandEncoder& commandEncoder) {
    _numOfdrawCall = 0;
    _shadowCount = 0;
    _drawSpotShadowMap(commandEncoder);
    _drawDirectShadowMap(commandEncoder);
    _scene->shaderData.setData(_shadowCountProp, _shadowCount);
    if (_shadowCount) {
        if (!_packedTexture || _packedTexture->depthOrArrayLayers() != _shadowCount) {
            _packedTexture = std::make_shared<SampledTexture2D>(_scene->device(), SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                                                _shadowCount, SHADOW_MAP_FORMAT);
            _packedTexture->setTextureViewDimension(wgpu::TextureViewDimension::e2DArray);
        }
        TextureUtils::buildTextureArray(_shadowMaps.begin(), _shadowMaps.begin() + _shadowCount,
                                        SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                        _packedTexture->texture(), commandEncoder);
        
        _scene->shaderData.setSampledTexture(_shadowMapProp, _shadowSamplerProp, _packedTexture);
        _scene->shaderData.setData(_shadowDataProp, _shadowDatas);
    }
    
    _cubeShadowCount = 0;
    _drawPointShadowMap(commandEncoder);
    _scene->shaderData.setData(_cubeShadowCountProp, _cubeShadowCount);
    if (_cubeShadowCount) {
        if (!_packedCubeTexture) {
            _packedCubeTexture = std::make_shared<SampledTextureCube>(_scene->device(), SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                                                      _cubeShadowCount, SHADOW_MAP_FORMAT);
            _packedCubeTexture->setTextureViewDimension(wgpu::TextureViewDimension::CubeArray);
        }
        TextureUtils::buildCubeTextureArray(_cubeShadowMaps.begin(), _cubeShadowMaps.begin() + _cubeShadowCount,
                                            SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                            _packedCubeTexture->texture(), commandEncoder);
        
        
        _scene->shaderData.setSampledTexture(_cubeShadowMapProp, _cubeShadowSamplerProp, _packedCubeTexture);
        _scene->shaderData.setData(_cubeShadowDataProp, _cubeShadowDatas);
    }
}

void ShadowManager::_drawSpotShadowMap(wgpu::CommandEncoder& commandEncoder) {
    const auto &lights = _scene->light_manager.spotLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _shadowCount < MAX_SHADOW) {
            wgpu::Texture texture = nullptr;
            if (_shadowCount < _shadowMaps.size()) {
                texture = _shadowMaps[_shadowCount];
            } else {
                wgpu::TextureDescriptor descriptor;
                descriptor.size.width = SHADOW_MAP_RESOLUTION;
                descriptor.size.height = SHADOW_MAP_RESOLUTION;
                descriptor.format = SHADOW_MAP_FORMAT;
                descriptor.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
                texture = _scene->device().CreateTexture(&descriptor);
                
                _shadowMaps.push_back(texture);
            }
            
            _updateSpotShadow(light, _shadowDatas[_shadowCount]);
            {
                _depthStencilAttachment.view = texture.CreateView();
                
                std::shared_ptr<ShadowMaterial> material{nullptr};
                if (_numOfdrawCall < _materialPool.size()) {
                    material = _materialPool[_numOfdrawCall];
                } else {
                    material = std::make_shared<ShadowMaterial>(_scene->device());
                    _materialPool.emplace_back(material);
                }
                material->setViewProjectionMatrix(_shadowDatas[_shadowCount].vp[0]);
                _shadowSubpass->setShadowMaterial(material);
                _renderPass->draw(commandEncoder, "Spot Shadow Pass");
                _numOfdrawCall++;
            }
            _shadowCount++;
        }
    }
}

void ShadowManager::_drawDirectShadowMap(wgpu::CommandEncoder& commandEncoder) {
    const auto &lights = _scene->light_manager.directLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _shadowCount < MAX_SHADOW) {
            wgpu::Texture texture = nullptr;
            if (_shadowCount < _shadowMaps.size()) {
                texture = _shadowMaps[_shadowCount];
            } else {
                wgpu::TextureDescriptor descriptor;
                descriptor.size.width = SHADOW_MAP_RESOLUTION;
                descriptor.size.height = SHADOW_MAP_RESOLUTION;
                descriptor.format = SHADOW_MAP_FORMAT;
                descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
                texture = _scene->device().CreateTexture(&descriptor);
                
                _shadowMaps.push_back(texture);
            }
            
            _updateCascadesShadow(light, _shadowDatas[_shadowCount]);
            for (int i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
                if (_cascadeShadowMaps[i] == nullptr) {
                    wgpu::TextureDescriptor descriptor;
                    descriptor.size.width = SHADOW_MAP_RESOLUTION / 2;
                    descriptor.size.height = SHADOW_MAP_RESOLUTION / 2;
                    descriptor.format = SHADOW_MAP_FORMAT;
                    descriptor.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
                    _cascadeShadowMaps[i] = _scene->device().CreateTexture(&descriptor);
                }
                
                _depthStencilAttachment.view = _cascadeShadowMaps[i].CreateView();
                
                std::shared_ptr<ShadowMaterial> material{nullptr};
                if (_numOfdrawCall < _materialPool.size()) {
                    material = _materialPool[_numOfdrawCall];
                } else {
                    material = std::make_shared<ShadowMaterial>(_scene->device());
                    _materialPool.emplace_back(material);
                }
                material->setViewProjectionMatrix(_shadowDatas[_shadowCount].vp[i]);
                _shadowSubpass->setShadowMaterial(material);
                _renderPass->draw(commandEncoder, "Direct Shadow Pass");
                _numOfdrawCall++;
            }
            
            TextureUtils::buildAtlas(_cascadeShadowMaps, SHADOW_MAP_RESOLUTION / 2, SHADOW_MAP_RESOLUTION / 2,
                                     texture, commandEncoder);
            _shadowCount++;
        }
    }
}

void ShadowManager::_drawPointShadowMap(wgpu::CommandEncoder& commandEncoder) {
    const auto &lights = _scene->light_manager.pointLights();
    for (const auto &light: lights) {
        if (light->enableShadow() && _cubeShadowCount < MAX_CUBE_SHADOW) {
            wgpu::Texture texture = nullptr;
            if (_cubeShadowCount < _cubeShadowMaps.size()) {
                texture = _cubeShadowMaps[_cubeShadowCount];
            } else {
                wgpu::TextureDescriptor descriptor;
                descriptor.size.width = SHADOW_MAP_RESOLUTION;
                descriptor.size.height = SHADOW_MAP_RESOLUTION;
                descriptor.size.depthOrArrayLayers = 6;
                descriptor.format = SHADOW_MAP_FORMAT;
                descriptor.usage = wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
                texture = _scene->device().CreateTexture(&descriptor);
                
                _cubeShadowMaps.push_back(texture);
            }
            
            _updatePointShadow(light, _cubeShadowDatas[_cubeShadowCount]);
            for (int i = 0; i < 6; i++) {
                if (_cubeMapSlices[i] == nullptr) {
                    wgpu::TextureDescriptor descriptor;
                    descriptor.size.width = SHADOW_MAP_RESOLUTION;
                    descriptor.size.height = SHADOW_MAP_RESOLUTION;
                    descriptor.format = SHADOW_MAP_FORMAT;
                    descriptor.usage = wgpu::TextureUsage::RenderAttachment | wgpu::TextureUsage::CopyDst | wgpu::TextureUsage::CopySrc;
                    _cubeMapSlices[i] = _scene->device().CreateTexture(&descriptor);
                }
                
                _depthStencilAttachment.view = _cubeMapSlices[i].CreateView();
                
                std::shared_ptr<ShadowMaterial> material{nullptr};
                if (_numOfdrawCall < _materialPool.size()) {
                    material = _materialPool[_numOfdrawCall];
                } else {
                    material = std::make_shared<ShadowMaterial>(_scene->device());
                    _materialPool.emplace_back(material);
                }
                material->setViewProjectionMatrix(_cubeShadowDatas[_cubeShadowCount].vp[i]);
                _shadowSubpass->setShadowMaterial(material);
                _renderPass->draw(commandEncoder, "Point Shadow Pass");
                _numOfdrawCall++;
            }
            TextureUtils::buildCubeAtlas(_cubeMapSlices, SHADOW_MAP_RESOLUTION, SHADOW_MAP_RESOLUTION,
                                         texture, commandEncoder);
            _cubeShadowCount++;
        }
    }
}

void ShadowManager::_updateSpotShadow(SpotLight* light, ShadowManager::ShadowData& shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    auto viewMatrix = light->entity()->transform->worldMatrix().inverse();
    auto projMatrix = light->shadowProjectionMatrix();
    shadowData.vp[0] = projMatrix * viewMatrix;
    shadowData.cascadeSplits[0] = 1;
    shadowData.cascadeSplits[1] = -1; // mark cascade with negative sign
}

void ShadowManager::_updateCascadesShadow(DirectLight *light, ShadowManager::ShadowData& shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    std::array<float, SHADOW_MAP_CASCADE_COUNT> cascadeSplits{};
    auto worldPos = light->entity()->transform->worldPosition();
    
    float nearClip = _camera->nearClipPlane();
    float farClip = _camera->farClipPlane();
    float clipRange = farClip - nearClip;
    
    float minZ = nearClip;
    float maxZ = nearClip + clipRange;
    
    float range = maxZ - minZ;
    float ratio = maxZ / minZ;
    
    // Calculate split depths based on view camera frustum
    // Based on method presented in https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch10.html
    for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
        float p = (i + 1) / static_cast<float>(SHADOW_MAP_CASCADE_COUNT);
        float log = minZ * std::pow(ratio, p);
        float uniform = minZ + range * p;
        float d = _cascadeSplitLambda * (log - uniform) + uniform;
        cascadeSplits[i] = (d - nearClip) / clipRange;
    }
    
    std::array<Point3F, 8> frustumCorners = {
        Point3F(-1.0f, 1.0f, 0.0f),
        Point3F(1.0f, 1.0f, 0.0f),
        Point3F(1.0f, -1.0f, 0.0f),
        Point3F(-1.0f, -1.0f, 0.0f),
        Point3F(-1.0f, 1.0f, 1.0f),
        Point3F(1.0f, 1.0f, 1.0f),
        Point3F(1.0f, -1.0f, 1.0f),
        Point3F(-1.0f, -1.0f, 1.0f),
    };
    
    // Project frustum corners into world space
    Matrix4x4F invCam = (_camera->projectionMatrix() * _camera->viewMatrix()).inverse();
    for (uint32_t i = 0; i < 8; i++) {
        frustumCorners[i] = invCam * frustumCorners[i];
    }
    
    // Calculate orthographic projection matrix for each cascade
    float lastSplitDist = 0.0;
    for (uint32_t i = 0; i < SHADOW_MAP_CASCADE_COUNT; i++) {
        float splitDist = cascadeSplits[i];
        std::array<Point3F, 8> _frustumCorners = frustumCorners;
        
        for (uint32_t i = 0; i < 4; i++) {
            Vector3F dist = _frustumCorners[i + 4] - _frustumCorners[i];
            _frustumCorners[i + 4] = _frustumCorners[i] + (dist * splitDist);
            _frustumCorners[i] = _frustumCorners[i] + (dist * lastSplitDist);
        }
        
        auto lightMat = light->entity()->transform->worldMatrix();
        auto lightViewMat = lightMat.inverse();
        for (uint32_t i = 0; i < 8; i++) {
            _frustumCorners[i] = lightViewMat * _frustumCorners[i];
        }
        float farDist = _frustumCorners[7].distanceTo(_frustumCorners[5]);
        float crossDist = _frustumCorners[7].distanceTo(_frustumCorners[1]);
        float maxDist = farDist > crossDist ? farDist : crossDist;
        
        float minX = std::numeric_limits<float>::infinity();
        float maxX = -std::numeric_limits<float>::infinity();
        float minY = std::numeric_limits<float>::infinity();
        float maxY = -std::numeric_limits<float>::infinity();
        float minZ = std::numeric_limits<float>::infinity();
        float maxZ = -std::numeric_limits<float>::infinity();
        for (uint32_t i = 0; i < 8; i++) {
            minX = std::min(minX, _frustumCorners[i].x);
            maxX = std::max(maxX, _frustumCorners[i].x);
            minY = std::min(minY, _frustumCorners[i].y);
            maxY = std::max(maxY, _frustumCorners[i].y);
            minZ = std::min(minZ, _frustumCorners[i].z);
            maxZ = std::max(maxZ, _frustumCorners[i].z);
        }
        
        // texel tile
        float fWorldUnitsPerTexel = maxDist / (float) 1000;
        float posX = (minX + maxX) * 0.5f;
        posX /= fWorldUnitsPerTexel;
        posX = std::floor(posX);
        posX *= fWorldUnitsPerTexel;
        
        float posY = (minY + maxY) * 0.5f;
        posY /= fWorldUnitsPerTexel;
        posY = std::floor(posY);
        posY *= fWorldUnitsPerTexel;
        
        float posZ = maxZ;
        posZ /= fWorldUnitsPerTexel;
        posZ = std::floor(posZ);
        posZ *= fWorldUnitsPerTexel;
        
        Point3F center = Point3F(posX, posY, posZ);
        center = lightMat * center;
        light->entity()->transform->setWorldPosition(center);
        
        float radius = maxDist / 2.0;
        Vector3F maxExtents = Vector3F(radius, radius, radius);
        Vector3F minExtents = -maxExtents;
        Matrix4x4F lightOrthoMatrix = makeOrtho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxZ - minZ);
        
        // Store split distance and matrix in cascade
        shadowData.cascadeSplits[i] = (_camera->nearClipPlane() + splitDist * clipRange) * -1.0f;
        shadowData.vp[i] = lightOrthoMatrix * light->entity()->transform->worldMatrix().inverse();
        light->entity()->transform->setWorldPosition(worldPos);
        lastSplitDist = cascadeSplits[i];
    }
}

void ShadowManager::_updatePointShadow(PointLight *light, ShadowManager::CubeShadowData& shadowData) {
    shadowData.radius = light->shadowRadius();
    shadowData.bias = light->shadowBias();
    shadowData.intensity = light->shadowIntensity();
    
    auto projMatrix = light->shadowProjectionMatrix();
    auto worldPos = light->entity()->transform->worldPosition();
    shadowData.lightPos = Vector4F(worldPos.x, worldPos.y, worldPos.z, 1.0);
    
    for (int i = 0; i < 6; i++) {
        light->entity()->transform->lookAt(worldPos + _cubeMapDirection[i].first, _cubeMapDirection[i].second);
        auto viewMatrix = light->entity()->transform->worldMatrix().inverse();
        shadowData.vp[i] = projMatrix * viewMatrix;
    }
}

}
