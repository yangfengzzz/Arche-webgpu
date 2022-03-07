//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "light_manager.h"
#include "shader/shader.h"
#include "scene.h"
#include "camera.h"
#include "lighting/wgsl/wgsl_cluster_compute.h"
#include <glog/logging.h>

namespace vox {
LightManager *LightManager::getSingletonPtr(void) {
    return msSingleton;
}

LightManager &LightManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}

LightManager::LightManager(Scene* scene) :
_scene(scene),
_shaderData(scene->device()),
_pointLightProperty(Shader::createProperty("u_pointLight", ShaderDataGroup::Scene)),
_spotLightProperty(Shader::createProperty("u_spotLight", ShaderDataGroup::Scene)),
_directLightProperty(Shader::createProperty("u_directLight", ShaderDataGroup::Scene)),
_projectionProp(Shader::createProperty("u_cluster_projection", ShaderDataGroup::Compute)),
_viewProp(Shader::createProperty("u_cluster_view", ShaderDataGroup::Compute)),
_clustersProp(Shader::createProperty("u_clusters", ShaderDataGroup::Compute)),
_clusterLightsProp(Shader::createProperty("u_clusterLight", ShaderDataGroup::Compute)) {
    auto& device = _scene->device();
    _clustersBuffer = std::make_unique<Buffer>(device, sizeof(Clusters), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    _shaderData.setBufferFunctor(_clustersProp, [this]()->Buffer {
        return *_clustersBuffer;
    });
    
    _clusterLightsBuffer = std::make_unique<Buffer>(device, sizeof(ClusterLightGroup), wgpu::BufferUsage::Storage | wgpu::BufferUsage::CopyDst);
    _shaderData.setBufferFunctor(_clusterLightsProp, [this]()->Buffer {
        return *_clusterLightsBuffer;
    });
    
    _clusterBoundsCompute =
    std::make_unique<ComputePass>(_scene->device(), std::make_unique<WGSLClusterBoundsSource>(TILE_COUNT, MAX_LIGHTS_PER_CLUSTER,
                                                                                              WORKGROUP_SIZE));
    _clusterBoundsCompute->attachShaderData(&_shaderData);
    
    _clusterLightsCompute =
    std::make_unique<ComputePass>(_scene->device(), std::make_unique<WGSLClusterLightsSource>(TILE_COUNT, MAX_LIGHTS_PER_CLUSTER,
                                                                                              WORKGROUP_SIZE));
    _clusterLightsCompute->attachShaderData(&_shaderData);
    _clusterLightsCompute->attachShaderData(&_scene->shaderData);
}

void LightManager::setCamera(Camera* camera) {
    _camera = camera;
}

//MARK: - Point Light
void LightManager::attachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter == _pointLights.end()) {
        _pointLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;;
    }
}

void LightManager::detachPointLight(PointLight *light) {
    auto iter = std::find(_pointLights.begin(), _pointLights.end(), light);
    if (iter != _pointLights.end()) {
        _pointLights.erase(iter);
    }
}

const std::vector<PointLight *> &LightManager::pointLights() const {
    return _pointLights;
}

//MARK: - Spot Light
void LightManager::attachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter == _spotLights.end()) {
        _spotLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;;
    }
}

void LightManager::detachSpotLight(SpotLight *light) {
    auto iter = std::find(_spotLights.begin(), _spotLights.end(), light);
    if (iter != _spotLights.end()) {
        _spotLights.erase(iter);
    }
}

const std::vector<SpotLight *> &LightManager::spotLights() const {
    return _spotLights;
}

//MARK: - Direct Light
void LightManager::attachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter == _directLights.end()) {
        _directLights.push_back(light);
    } else {
        LOG(ERROR) << "Light already attached." << std::endl;;
    }
}

void LightManager::detachDirectLight(DirectLight *light) {
    auto iter = std::find(_directLights.begin(), _directLights.end(), light);
    if (iter != _directLights.end()) {
        _directLights.erase(iter);
    }
}

const std::vector<DirectLight *> &LightManager::directLights() const {
    return _directLights;
}

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
        shaderData.enableMacro(DIRECT_LIGHT_COUNT, directLightCount);
        shaderData.setData(LightManager::_directLightProperty, _directLightDatas);
    } else {
        shaderData.disableMacro(DIRECT_LIGHT_COUNT);
    }
    
    if (pointLightCount) {
        shaderData.enableMacro(POINT_LIGHT_COUNT, pointLightCount);
        shaderData.setData(LightManager::_pointLightProperty, _pointLightDatas);
    } else {
        shaderData.disableMacro(POINT_LIGHT_COUNT);
    }
    
    if (spotLightCount) {
        shaderData.enableMacro(SPOT_LIGHT_COUNT, spotLightCount);
        shaderData.setData(LightManager::_spotLightProperty, _spotLightDatas);
    } else {
        shaderData.disableMacro(SPOT_LIGHT_COUNT);
    }
}

//MARK: - Forward Plus
void LightManager::draw(wgpu::CommandEncoder& commandEncoder) {
    _updateShaderData(_scene->shaderData);
    
    size_t pointLightCount = _pointLights.size();
    size_t spotLightCount = _spotLights.size();
    if (pointLightCount + spotLightCount > FORWARD_PLUS_ENABLE) {
        bool updateBounds = false;
        
        _projectionUniforms.matrix = _camera->projectionMatrix();
        _projectionUniforms.inverseMatrix = _camera->inverseProjectionMatrix();
        if (_projectionUniforms.outputSize.x != _camera->width() ||
            _projectionUniforms.outputSize.y != _camera->height()) {
            updateBounds = true;
            _projectionUniforms.outputSize = Vector2F(_camera->width(), _camera->height());
        }
        _projectionUniforms.zNear = _camera->nearClipPlane();
        _projectionUniforms.zFar = _camera->farClipPlane();
        _shaderData.setData(_projectionProp, _projectionUniforms);
        
        auto encoder = commandEncoder.BeginComputePass();
        if (updateBounds) {
            _clusterBoundsCompute->compute(encoder);
        }
        _clusterLightsCompute->compute(encoder);
        encoder.End();
    }
}


}
