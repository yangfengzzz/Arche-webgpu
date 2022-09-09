//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/lighting//light_manager.h"

#include "vox.base/logging.h"
#include "vox.render/camera.h"
#include "vox.render/shader/internal_variant_name.h"
#include "vox.render/shader/shader_manager.h"

namespace vox {
LightManager *LightManager::getSingletonPtr() { return ms_singleton; }

LightManager &LightManager::getSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

LightManager::LightManager(Scene *scene)
    : _scene(scene),
      _shaderData(scene->device()),
      _pointLightProperty("u_pointLight"),
      _spotLightProperty("u_spotLight"),
      _directLightProperty("u_directLight"),
      _forwardPlusProp("u_cluster_uniform"),
      _clustersProp("u_clusters"),
      _clusterLightsProp("u_clusterLights") {
    auto &device = _scene->device();
    _clustersBuffer =
            std::make_unique<Buffer>(device, sizeof(Clusters), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    _shaderData.setBufferFunctor(_clustersProp, [this]() -> Buffer { return *_clustersBuffer; });

    _clusterLightsBuffer = std::make_unique<Buffer>(device, sizeof(ClusterLightGroup),
                                                    wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    _scene->shaderData.setBufferFunctor(_clusterLightsProp, [this]() -> Buffer { return *_clusterLightsBuffer; });

    _clusterBoundsCompute = std::make_unique<ComputePass>(
            device, ShaderManager::GetSingleton().LoadShader("light/cluster_bounds.comp"));
    _clusterBoundsCompute->attachShaderData(&_shaderData);
    _clusterBoundsCompute->attachShaderData(&_scene->shaderData);
    _clusterBoundsCompute->setDispatchCount(DISPATCH_SIZE[0], DISPATCH_SIZE[1], DISPATCH_SIZE[2]);

    _clusterLightsCompute = std::make_unique<ComputePass>(
            device, ShaderManager::GetSingleton().LoadShader("light/cluster_light.comp"));
    _clusterLightsCompute->attachShaderData(&_shaderData);
    _clusterLightsCompute->attachShaderData(&_scene->shaderData);
    _clusterLightsCompute->setDispatchCount(DISPATCH_SIZE[0], DISPATCH_SIZE[1], DISPATCH_SIZE[2]);
}

void LightManager::setCamera(Camera *camera) { _camera = camera; }

// MARK: - Point Light
void LightManager::attachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter == _pointLights.end()) {
        _pointLights.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter != _pointLights.end()) {
        _pointLights.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::pointLights() const { return _pointLights; }

// MARK: - Spot Light
void LightManager::attachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter == _spotLights.end()) {
        _spotLights.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter != _spotLights.end()) {
        _spotLights.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::spotLights() const { return _spotLights; }

// MARK: - Direct Light
void LightManager::attachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter == _directLights.end()) {
        _directLights.push_back(light);
    } else {
        LOGE("Light already attached.")
    }
}

void LightManager::detachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter != _directLights.end()) {
        _directLights.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::directLights() const { return _directLights; }

void LightManager::_updateShaderData(ShaderData &shaderData) {
    size_t pointLightCount = _pointLights.size();
    _pointLightDatas.resize(pointLightCount);
    size_t spotLightCount = _spotLights.size();
    _spotLightDatas.resize(spotLightCount);
    size_t directLightCount = _directLights.size();
    _directLightDatas.resize(directLightCount);

    for (size_t i = 0; i < pointLightCount; i++) {
        _pointLights[i]->_updateShaderData(_pointLightDatas[i]);
    }

    for (size_t i = 0; i < spotLightCount; i++) {
        _spotLights[i]->_updateShaderData(_spotLightDatas[i]);
    }

    for (size_t i = 0; i < directLightCount; i++) {
        _directLights[i]->_updateShaderData(_directLightDatas[i]);
    }

    if (directLightCount) {
        shaderData.addDefine(DIRECT_LIGHT_COUNT + std::to_string(directLightCount));
        shaderData.setData(LightManager::_directLightProperty, _directLightDatas);
    } else {
        shaderData.removeDefine(DIRECT_LIGHT_COUNT);
    }

    if (pointLightCount) {
        shaderData.addDefine(POINT_LIGHT_COUNT + std::to_string(pointLightCount));
        shaderData.setData(LightManager::_pointLightProperty, _pointLightDatas);
    } else {
        shaderData.removeDefine(POINT_LIGHT_COUNT);
    }

    if (spotLightCount) {
        shaderData.addDefine(SPOT_LIGHT_COUNT + std::to_string(spotLightCount));
        shaderData.setData(LightManager::_spotLightProperty, _spotLightDatas);
    } else {
        shaderData.removeDefine(SPOT_LIGHT_COUNT);
    }
}

// MARK: - Forward Plus
bool LightManager::enableForwardPlus() const { return _enableForwardPlus; }

void LightManager::draw(wgpu::CommandEncoder &commandEncoder) {
    _updateShaderData(_scene->shaderData);

    size_t pointLightCount = _pointLights.size();
    size_t spotLightCount = _spotLights.size();
    if (pointLightCount + spotLightCount > FORWARD_PLUS_ENABLE_MIN_COUNT) {
        _enableForwardPlus = true;
        bool updateBounds = false;

        _forwardPlusUniforms.matrix = _camera->projectionMatrix();
        _forwardPlusUniforms.inverseMatrix = _camera->inverseProjectionMatrix();
        if (_forwardPlusUniforms.outputSize.x != _camera->width() ||
            _forwardPlusUniforms.outputSize.y != _camera->height()) {
            updateBounds = true;
            _forwardPlusUniforms.outputSize = Vector2F(_camera->framebufferWidth(), _camera->framebufferHeight());
        }
        _forwardPlusUniforms.zNear = _camera->nearClipPlane();
        _forwardPlusUniforms.zFar = _camera->farClipPlane();
        _forwardPlusUniforms.viewMatrix = _camera->viewMatrix();
        _scene->shaderData.setData(_forwardPlusProp, _forwardPlusUniforms);

        // Reset the light offset counter to 0 before populating the light clusters.
        uint32_t empty = 0;
        _clusterLightsBuffer->uploadData(_scene->device(), &empty, sizeof(uint32_t));

        auto encoder = commandEncoder.BeginComputePass();
        if (updateBounds) {
            _clusterBoundsCompute->compute(encoder);
        }
        _clusterLightsCompute->compute(encoder);
        encoder.End();
    }
}

}  // namespace vox
