//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "scene_animator.h"
#include "entity.h"
#include "scene.h"
#include "components_manager.h"
#include <iostream>

namespace vox {
std::string SceneAnimator::name() {
    return "SceneAnimator";
}

SceneAnimator::SceneAnimator(Entity *entity) :
Component(entity) {
}

void SceneAnimator::update(float deltaTime) {
    if (_activeAnimation != -1) {
        _animationClips[_activeAnimation]->update(deltaTime);
    }
}

void SceneAnimator::addAnimationClip(std::unique_ptr<SceneAnimationClip> &&clip) {
    _animationClips.emplace_back(std::move(clip));
}

void SceneAnimator::play(const std::string &name) {
    auto iter = std::find_if(_animationClips.begin(), _animationClips.end(), [&](const auto &u) {
        return u->name() == name;
    });
    if (iter != _animationClips.end()) {
        _activeAnimation = iter - _animationClips.begin();
    } else {
        _activeAnimation = -1;
    }
}

void SceneAnimator::_onEnable() {
    ComponentsManager::getSingleton().addOnUpdateSceneAnimators(this);
}

void SceneAnimator::_onDisable() {
    ComponentsManager::getSingleton().removeOnUpdateSceneAnimators(this);
}

//MARK: - Reflection
void SceneAnimator::onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void SceneAnimator::onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void SceneAnimator::onInspector(ui::WidgetContainer& p_root) {
    
}

}
