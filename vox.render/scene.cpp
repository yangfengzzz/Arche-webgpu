//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/scene.h"

#include <queue>

#include "vox.base/logging.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"

namespace vox {
Scene::Scene(wgpu::Device &device) : _device(device), shaderData(device) {
    setAmbientLight(std::make_shared<AmbientLight>());
}

Scene::~Scene() { _rootEntities.clear(); }

wgpu::Device &Scene::device() { return _device; }

const std::shared_ptr<AmbientLight> &Scene::ambientLight() const { return _ambientLight; }

void Scene::setAmbientLight(const std::shared_ptr<AmbientLight> &light) {
    if (!light) {
        LOGE("The scene must have one ambient light")
        return;
    }

    auto lastAmbientLight = _ambientLight;
    if (lastAmbientLight != light) {
        light->setScene(this);
        _ambientLight = light;
    }
}

size_t Scene::rootEntitiesCount() { return _rootEntities.size(); }

const std::vector<std::unique_ptr<Entity>> &Scene::rootEntities() const { return _rootEntities; }

void Scene::play() { _processActive(true); }

bool Scene::isPlaying() const { return _isActiveInEngine; }

Entity *Scene::createRootEntity(const std::string &name) {
    auto entity = std::make_unique<Entity>(name);
    auto entityPtr = entity.get();
    addRootEntity(std::move(entity));
    return entityPtr;
}

void Scene::addRootEntity(std::unique_ptr<Entity> &&entity) {
    const auto isRoot = entity->_isRoot;

    // let entity become root
    if (!isRoot) {
        entity->_isRoot = true;
        entity->_removeFromParent();
    }

    // add or remove from scene's rootEntities
    Entity *entityPtr = entity.get();
    const auto oldScene = entity->_scene;
    if (oldScene != this) {
        if (oldScene && isRoot) {
            oldScene->_removeEntity(entityPtr);
        }
        Entity::_traverseSetOwnerScene(entityPtr, this);
        _rootEntities.emplace_back(std::move(entity));
    } else if (!isRoot) {
        _rootEntities.emplace_back(std::move(entity));
    }

    // process entity active/inActive
    if (_isActiveInEngine) {
        if (!entityPtr->_isActiveInHierarchy && entityPtr->_isActive) {
            entityPtr->_processActive();
        }
    } else {
        if (entityPtr->_isActiveInHierarchy) {
            entityPtr->_processInActive();
        }
    }
}

void Scene::removeRootEntity(Entity *entity) {
    if (entity->_isRoot && entity->_scene == this) {
        _removeEntity(entity);
        if (_isActiveInEngine) {
            entity->_processInActive();
        }
        Entity::_traverseSetOwnerScene(entity, nullptr);
    }
}

Entity *Scene::getRootEntity(size_t index) { return _rootEntities[index].get(); }

Entity *Scene::findEntityByName(const std::string &name) {
    const auto &children = _rootEntities;
    for (const auto &child : children) {
        if (child->name == name) {
            return child.get();
        }
    }

    for (const auto &child : children) {
        const auto entity = child->findByName(name);
        if (entity) {
            return entity;
        }
    }
    return nullptr;
}

void Scene::attachRenderCamera(Camera *camera) {
    auto iter = std::find(_activeCameras.begin(), _activeCameras.end(), camera);
    if (iter == _activeCameras.end()) {
        _activeCameras.push_back(camera);
    } else {
        LOGI("Camera already attached.")
    }
}

void Scene::detachRenderCamera(Camera *camera) {
    auto iter = std::find(_activeCameras.begin(), _activeCameras.end(), camera);
    if (iter != _activeCameras.end()) {
        _activeCameras.erase(iter);
    }
}

void Scene::_processActive(bool active) {
    _isActiveInEngine = active;
    const auto &rootEntities = _rootEntities;
    for (const auto &entity : rootEntities) {
        if (entity->_isActive) {
            active ? entity->_processActive() : entity->_processInActive();
        }
    }
}

void Scene::_removeEntity(Entity *entity) {
    auto &oldRootEntities = _rootEntities;
    oldRootEntities.erase(std::remove_if(oldRootEntities.begin(), oldRootEntities.end(),
                                         [entity](auto &oldEntity) { return oldEntity.get() == entity; }),
                          oldRootEntities.end());
}

// MARK: - Update Loop
void Scene::updateShaderData() {
    // union scene and camera macro.
    for (auto &camera : _activeCameras) {
        camera->update();
    }
}

// MARK: - Reflection
void Scene::onSerialize(nlohmann::json &data) {}

void Scene::onDeserialize(nlohmann::json &data) {}

}  // namespace vox
