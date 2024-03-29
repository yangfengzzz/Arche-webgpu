//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/components_manager.h"

#include "vox.base/logging.h"
#include "vox.render/animation/animator.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/renderer.h"
#include "vox.render/script.h"

namespace vox {
ComponentsManager *ComponentsManager::getSingletonPtr() { return ms_singleton; }

ComponentsManager &ComponentsManager::getSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

void ComponentsManager::addOnStartScript(Script *script) {
    auto iter = std::find(_onStartScripts.begin(), _onStartScripts.end(), script);
    if (iter == _onStartScripts.end()) {
        _onStartScripts.push_back(script);
    } else {
        LOGE("Script already attached.")
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
        LOGE("Script already attached.")
    }
}

void ComponentsManager::removeOnUpdateScript(Script *script) {
    auto iter = std::find(_onUpdateScripts.begin(), _onUpdateScripts.end(), script);
    if (iter != _onUpdateScripts.end()) {
        _onUpdateScripts.erase(iter);
    }
}

void ComponentsManager::addDestroyComponent(Script *component) { _destroyComponents.push_back(component); }

void ComponentsManager::callComponentDestroy() {
    if (!_destroyComponents.empty()) {
        for (auto &_destroyComponent : _destroyComponents) {
            _destroyComponent->onDestroy();
        }
        _destroyComponents.clear();
    }
}

void ComponentsManager::callScriptOnStart() {
    if (!_onStartScripts.empty()) {
        // The 'onStartScripts.length' maybe add if you add some Script with addComponent() in some Script's onStart()
        for (auto &script : _onStartScripts) {
            script->setIsStarted(true);
            script->onStart();
        }
        _onStartScripts.clear();
    }
}

void ComponentsManager::callScriptOnUpdate(float deltaTime) {
    for (auto &element : _onUpdateScripts) {
        if (element->isStarted()) {
            element->onUpdate(deltaTime);
        }
    }
}

void ComponentsManager::callScriptOnLateUpdate(float deltaTime) {
    for (auto &element : _onUpdateScripts) {
        if (element->isStarted()) {
            element->onLateUpdate(deltaTime);
        }
    }
}

void ComponentsManager::callScriptInputEvent(const InputEvent &inputEvent) {
    for (auto &element : _onUpdateScripts) {
        if (element->isStarted()) {
            element->inputEvent(inputEvent);
        }
    }
}

void ComponentsManager::callScriptResize(uint32_t win_width,
                                         uint32_t win_height,
                                         uint32_t fb_width,
                                         uint32_t fb_height) {
    for (auto &element : _onUpdateScripts) {
        if (element->isStarted()) {
            element->resize(win_width, win_height, fb_width, fb_height);
        }
    }
}

// MARK: - Renderer
void ComponentsManager::addRenderer(Renderer *renderer) {
    auto iter = std::find(_renderers.begin(), _renderers.end(), renderer);
    if (iter == _renderers.end()) {
        _renderers.push_back(renderer);
    } else {
        LOGE("Renderer already attached.")
    }
}

void ComponentsManager::removeRenderer(Renderer *renderer) {
    auto iter = std::find(_renderers.begin(), _renderers.end(), renderer);
    if (iter != _renderers.end()) {
        _renderers.erase(iter);
    }
}

void ComponentsManager::callRendererOnUpdate(float deltaTime) {
    for (auto &_renderer : _renderers) {
        _renderer->update(deltaTime);
    }
}

void ComponentsManager::callRender(const BoundingFrustum &frustum,
                                   std::vector<RenderElement> &opaqueQueue,
                                   std::vector<RenderElement> &alphaTestQueue,
                                   std::vector<RenderElement> &transparentQueue) {
    for (auto &renderer : _renderers) {
        // filter by renderer castShadow and frustum cull
        if (frustum.intersectsBox(renderer->bounds())) {
            renderer->render(opaqueQueue, alphaTestQueue, transparentQueue);
        }
    }
}

// MARK: - Camera
void ComponentsManager::callCameraOnBeginRender(Camera *camera) {
    const auto &camComps = camera->entity()->scripts();
    for (auto camComp : camComps) {
        camComp->onBeginRender(camera);
    }
}

void ComponentsManager::callCameraOnEndRender(Camera *camera) {
    const auto &camComps = camera->entity()->scripts();
    for (auto camComp : camComps) {
        camComp->onEndRender(camera);
    }
}

std::vector<Component *> ComponentsManager::getActiveChangedTempList() {
    return !_componentsContainerPool.empty() ? *(_componentsContainerPool.end() - 1) : std::vector<Component *>{};
}

void ComponentsManager::putActiveChangedTempList(std::vector<Component *> &componentContainer) {
    componentContainer.clear();
    _componentsContainerPool.push_back(componentContainer);
}

// MARK: - Animation
void ComponentsManager::addOnUpdateAnimators(Animator *animator) {
    auto iter = std::find(_onUpdateAnimators.begin(), _onUpdateAnimators.end(), animator);
    if (iter == _onUpdateAnimators.end()) {
        _onUpdateAnimators.push_back(animator);
    } else {
        LOGE("Animator already attached.")
    }
}

void ComponentsManager::removeOnUpdateAnimators(Animator *animator) {
    auto iter = std::find(_onUpdateAnimators.begin(), _onUpdateAnimators.end(), animator);
    if (iter != _onUpdateAnimators.end()) {
        _onUpdateAnimators.erase(iter);
    }
}

void ComponentsManager::callAnimatorUpdate(float deltaTime) {
    const auto &elements = _onUpdateAnimators;
    for (size_t i = 0; i < _onUpdateAnimators.size(); i++) {
        elements[i]->update(deltaTime);
    }
}

}  // namespace vox
