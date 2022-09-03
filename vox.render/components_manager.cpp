//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "components_manager.h"
#include "script.h"
#include "renderer.h"
#include "entity.h"
#include "camera.h"
#include "scene_animator.h"
#include <glog/logging.h>

namespace vox {
ComponentsManager *ComponentsManager::getSingletonPtr(void) {
    return msSingleton;
}

ComponentsManager &ComponentsManager::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}

void ComponentsManager::addOnStartScript(Script *script) {
    auto iter = std::find(_onStartScripts.begin(), _onStartScripts.end(), script);
    if (iter == _onStartScripts.end()) {
        _onStartScripts.push_back(script);
    } else {
        LOG(ERROR) << "Script already attached." << std::endl;;
    }
}

void ComponentsManager::removeOnStartScript(Script *script) {
    auto iter = std::find(_onStartScripts.begin(), _onStartScripts.end(), script);
    if (iter != _onStartScripts.end()) {
        _onStartScripts.erase(iter);
    }
}

void ComponentsManager::addOnUpdateScript(Script *script) {
    auto iter = std::find(_onUpdateScripts.begin(), _onUpdateScripts.end(), script);
    if (iter == _onUpdateScripts.end()) {
        _onUpdateScripts.push_back(script);
    } else {
        LOG(ERROR) << "Script already attached." << std::endl;;
    }
}

void ComponentsManager::removeOnUpdateScript(Script *script) {
    auto iter = std::find(_onUpdateScripts.begin(), _onUpdateScripts.end(), script);
    if (iter != _onUpdateScripts.end()) {
        _onUpdateScripts.erase(iter);
    }
}

void ComponentsManager::addDestroyComponent(Script *component) {
    _destroyComponents.push_back(component);
}

void ComponentsManager::callComponentDestroy() {
    if (_destroyComponents.size() > 0) {
        for (size_t i = 0; i < _destroyComponents.size(); i++) {
            _destroyComponents[i]->onDestroy();
        }
        _destroyComponents.clear();
    }
}

void ComponentsManager::callScriptOnStart() {
    if (_onStartScripts.size() > 0) {
        // The 'onStartScripts.length' maybe add if you add some Script with addComponent() in some Script's onStart()
        for (size_t i = 0; i < _onStartScripts.size(); i++) {
            const auto &script = _onStartScripts[i];
            script->setIsStarted(true);
            script->onStart();
        }
        _onStartScripts.clear();
    }
}

void ComponentsManager::callScriptOnUpdate(float deltaTime) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->isStarted()) {
            element->onUpdate(deltaTime);
        }
    }
}

void ComponentsManager::callScriptOnLateUpdate(float deltaTime) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->isStarted()) {
            element->onLateUpdate(deltaTime);
        }
    }
}

void ComponentsManager::callScriptInputEvent(const InputEvent &inputEvent) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->isStarted()) {
            element->inputEvent(inputEvent);
        }
    }
}

void ComponentsManager::callScriptResize(uint32_t win_width, uint32_t win_height,
                                         uint32_t fb_width, uint32_t fb_height) {
    for (size_t i = 0; i < _onUpdateScripts.size(); i++) {
        const auto &element = _onUpdateScripts[i];
        if (element->isStarted()) {
            element->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

//MARK: - Renderer
void ComponentsManager::addRenderer(Renderer *renderer) {
    auto iter = std::find(_renderers.begin(), _renderers.end(), renderer);
    if (iter == _renderers.end()) {
        _renderers.push_back(renderer);
    } else {
        LOG(ERROR) << "Renderer already attached." << std::endl;;
    }
}

void ComponentsManager::removeRenderer(Renderer *renderer) {
    auto iter = std::find(_renderers.begin(), _renderers.end(), renderer);
    if (iter != _renderers.end()) {
        _renderers.erase(iter);
    }
}

void ComponentsManager::callRendererOnUpdate(float deltaTime) {
    for (size_t i = 0; i < _renderers.size(); i++) {
        _renderers[i]->update(deltaTime);
    }
}

void ComponentsManager::callRender(Camera *camera,
                                   std::vector<RenderElement> &opaqueQueue,
                                   std::vector<RenderElement> &alphaTestQueue,
                                   std::vector<RenderElement> &transparentQueue) {
    for (size_t i = 0; i < _renderers.size(); i++) {
        const auto &element = _renderers[i];
        
        // filter by camera culling mask.
        if (!(camera->cullingMask & element->_entity->layer)) {
            continue;
        }
        
        // filter by camera frustum.
        if (camera->enableFrustumCulling) {
            element->isCulled = !camera->frustum().intersectsBox(element->bounds());
            if (element->isCulled) {
                continue;
            }
        }
        
        const auto &transform = camera->entity()->transform;
        const auto position = transform->worldPosition();
        auto center = element->bounds().midPoint();
        if (camera->isOrthographic()) {
            const auto forward = transform->worldForward();
            const auto offset = center - position;
            element->setDistanceForSort(offset.dot(forward));
        } else {
            element->setDistanceForSort(center.distanceSquaredTo(position));
        }
        
        element->_render(opaqueQueue, alphaTestQueue, transparentQueue);
    }
}

void ComponentsManager::callRender(const BoundingFrustum &frustrum,
                                   std::vector<RenderElement> &opaqueQueue,
                                   std::vector<RenderElement> &alphaTestQueue,
                                   std::vector<RenderElement> &transparentQueue) {
    for (size_t i = 0; i < _renderers.size(); i++) {
        const auto &renderer = _renderers[i];
        // filter by renderer castShadow and frustrum cull
        if (frustrum.intersectsBox(renderer->bounds())) {
            renderer->_render(opaqueQueue, alphaTestQueue, transparentQueue);
        }
    }
}

//MARK: - Camera
void ComponentsManager::callCameraOnBeginRender(Camera *camera) {
    const auto &camComps = camera->entity()->scripts();
    for (size_t i = 0; i < camComps.size(); i++) {
        camComps[i]->onBeginRender(camera);
    }
}

void ComponentsManager::callCameraOnEndRender(Camera *camera) {
    const auto &camComps = camera->entity()->scripts();
    for (size_t i = 0; i < camComps.size(); i++) {
        camComps[i]->onEndRender(camera);
    }
}

std::vector<Component *> ComponentsManager::getActiveChangedTempList() {
    return _componentsContainerPool.size() ? *(_componentsContainerPool.end() - 1) : std::vector<Component *>{};
}

void ComponentsManager::putActiveChangedTempList(std::vector<Component *> &componentContainer) {
    componentContainer.clear();
    _componentsContainerPool.push_back(componentContainer);
}

//MARK: - Animation
void ComponentsManager::addOnUpdateSceneAnimators(SceneAnimator *animator) {
    auto iter = std::find(_onUpdateSceneAnimators.begin(), _onUpdateSceneAnimators.end(), animator);
    if (iter == _onUpdateSceneAnimators.end()) {
        _onUpdateSceneAnimators.push_back(animator);
    } else {
        LOG(ERROR) << "SceneAnimator already attached." << std::endl;;
    }
}

void ComponentsManager::removeOnUpdateSceneAnimators(SceneAnimator *animator) {
    auto iter = std::find(_onUpdateSceneAnimators.begin(), _onUpdateSceneAnimators.end(), animator);
    if (iter != _onUpdateSceneAnimators.end()) {
        _onUpdateSceneAnimators.erase(iter);
    }
}

void ComponentsManager::callSceneAnimatorUpdate(float deltaTime) {
    const auto &elements = _onUpdateSceneAnimators;
    for (size_t i = 0; i < _onUpdateSceneAnimators.size(); i++) {
        elements[i]->update(deltaTime);
    }
}

}        // namespace vox
