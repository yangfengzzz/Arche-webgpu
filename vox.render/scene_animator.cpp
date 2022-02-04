//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.


#include "scene_animator.h"
#include "entity.h"
#include "scene.h"
#include <iostream>

namespace vox {
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
    scene()->_componentsManager.addOnUpdateSceneAnimators(this);
}

void SceneAnimator::_onDisable() {
    scene()->_componentsManager.removeOnUpdateSceneAnimators(this);
}

}
