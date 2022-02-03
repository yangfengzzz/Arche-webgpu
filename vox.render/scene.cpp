//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scene.h"
#include "vector2.h"
#include "vector3.h"
#include "vector4.h"
#include "color.h"
#include "matrix4x4.h"

#include <queue>
#include <glog/logging.h>
#include "entity.h"
#include "camera.h"

namespace vox {
Scene::Scene(wgpu::Device &device) :
_device(device),
shaderData(device),
_ambientLight(this) {
}

wgpu::Device &Scene::device() {
    return _device;
}

AmbientLight &Scene::ambientLight() {
    return _ambientLight;
}

size_t Scene::rootEntitiesCount() {
    return _rootEntities.size();
}

const std::vector<EntityPtr> &Scene::rootEntities() {
    return _rootEntities;
}

bool Scene::destroyed() {
    return _destroyed;
}

EntityPtr Scene::createRootEntity(std::string name) {
    const auto entity = std::make_shared<Entity>(name);
    addRootEntity(entity);
    return entity;
}

void Scene::addRootEntity(EntityPtr entity) {
    const auto isRoot = entity->_isRoot;
    
    // let entity become root
    if (!isRoot) {
        entity->_isRoot = true;
        entity->_removeFromParent();
    }
    
    // add or remove from scene's rootEntities
    const auto oldScene = entity->_scene;
    if (oldScene != this) {
        if (oldScene && isRoot) {
            oldScene->_removeEntity(entity);
        }
        _rootEntities.push_back(entity);
        Entity::_traverseSetOwnerScene(entity.get(), this);
    } else if (!isRoot) {
        _rootEntities.push_back(entity);
    }
    
    // process entity active/inActive
    if (!entity->_isActiveInHierarchy && entity->_isActive) {
        entity->_processActive();
    }
}

void Scene::removeRootEntity(EntityPtr entity) {
    if (entity->_isRoot && entity->_scene == this) {
        _removeEntity(entity);
        entity->_processInActive();
        Entity::_traverseSetOwnerScene(entity.get(), nullptr);
    }
}

EntityPtr Scene::getRootEntity(size_t index) {
    return _rootEntities[index];
}

EntityPtr Scene::findEntityByName(const std::string &name) {
    const auto &children = _rootEntities;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        if (child->name == name) {
            return child;
        }
    }
    
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        const auto entity = child->findByName(name);
        if (entity) {
            return entity;
        }
    }
    return nullptr;
}

void Scene::destroy() {
    if (_destroyed) {
        return;
    }
    for (size_t i = 0, n = rootEntitiesCount(); i < n; i++) {
        _rootEntities[i]->destroy();
    }
    _rootEntities.clear();
    _destroyed = true;
}

void Scene::attachRenderCamera(Camera *camera) {
    auto iter = std::find(_activeCameras.begin(), _activeCameras.end(), camera);
    if (iter == _activeCameras.end()) {
        _activeCameras.push_back(camera);
    } else {
        LOG(INFO) << "Camera already attached." << std::endl;
    }
}

void Scene::detachRenderCamera(Camera *camera) {
    auto iter = std::find(_activeCameras.begin(), _activeCameras.end(), camera);
    if (iter != _activeCameras.end()) {
        _activeCameras.erase(iter);
    }
}

void Scene::_processActive(bool active) {
    const auto &rootEntities = _rootEntities;
    for (size_t i = 0; i < rootEntities.size(); i++) {
        const auto &entity = rootEntities[i];
        if (entity->_isActive) {
            active ? entity->_processActive() : entity->_processInActive();
        }
    }
}

void Scene::_removeEntity(EntityPtr entity) {
    auto &oldRootEntities = _rootEntities;
    oldRootEntities.erase(std::remove(oldRootEntities.begin(),
                                      oldRootEntities.end(), entity), oldRootEntities.end());
}

//MARK: - Update Loop
void Scene::updateShaderData() {
    // union scene and camera macro.
    light_manager.updateShaderData(_device, shaderData);
    for (auto& camera : _activeCameras) {
        camera->updateShaderData();
    }
}

void Scene::update(float deltaTime) {
    _componentsManager.callScriptOnStart();
    
//    _physicsManager.callColliderOnUpdate();
//    _physicsManager.update(deltaTime);
//    _physicsManager.callColliderOnLateUpdate();
//    _physicsManager.callCharacterControllerOnLateUpdate();
    
    _componentsManager.callScriptOnUpdate(deltaTime);
//    _componentsManager.callAnimatorUpdate(deltaTime);
//    _componentsManager.callSceneAnimatorUpdate(deltaTime);
    _componentsManager.callScriptOnLateUpdate(deltaTime);
    
    _componentsManager.callRendererOnUpdate(deltaTime);
}

void Scene::updateInputEvent(const InputEvent &inputEvent) {
    _componentsManager.callScriptInputEvent(inputEvent);
}

void Scene::updateSize(uint32_t win_width, uint32_t win_height,
                       uint32_t fb_width, uint32_t fb_height) {
    _componentsManager.callScriptResize(win_width, win_height,
                                        fb_width, fb_height);
}

}        // namespace vox
