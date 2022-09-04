//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/scene_manager.h"

#include <utility>

namespace vox {
SceneManager::SceneManager(wgpu::Device &device, std::string p_sceneRootFolder)
    : _device(device), _sceneRootFolder(std::move(p_sceneRootFolder)) {
    loadEmptyScene();
}

SceneManager::~SceneManager() { unloadCurrentScene(); }

void SceneManager::update() {
    if (_delayedLoadCall) {
        _delayedLoadCall();
        _delayedLoadCall = nullptr;
    }
}

void SceneManager::loadAndPlayDelayed(const std::string &p_path, bool p_absolute) {
    _delayedLoadCall = [this, p_path, p_absolute] {
        std::string previousSourcePath = currentSceneSourcePath();
        loadScene(p_path, p_absolute);
        storeCurrentSceneSourcePath(previousSourcePath);
        currentScene()->play();
    };
}

void SceneManager::loadEmptyScene() {
    unloadCurrentScene();

    _currentScene = std::make_unique<Scene>(_device);
    _currentScene->_processActive(false);

    sceneLoadEvent.Invoke();
}

bool SceneManager::loadScene(const std::string &p_path, bool p_absolute) { return false; }

bool SceneManager::loadSceneFromMemory(const nlohmann::json &data) { return false; }

void SceneManager::unloadCurrentScene() {
    if (_currentScene) {
        _currentScene.reset();
        _currentScene = nullptr;
        sceneUnloadEvent.Invoke();
    }

    forgetCurrentSceneSourcePath();
}

bool SceneManager::hasCurrentScene() const { return _currentScene != nullptr; }

Scene *SceneManager::currentScene() { return _currentScene.get(); }

std::string SceneManager::currentSceneSourcePath() const { return _currentSceneSourcePath; }

bool SceneManager::isCurrentSceneLoadedFromDisk() const { return _currentSceneLoadedFromPath; }

void SceneManager::storeCurrentSceneSourcePath(const std::string &p_path) {
    _currentSceneSourcePath = p_path;
    _currentSceneLoadedFromPath = true;
    currentSceneSourcePathChangedEvent.Invoke(_currentSceneSourcePath);
}

void SceneManager::forgetCurrentSceneSourcePath() {
    _currentSceneSourcePath = "";
    _currentSceneLoadedFromPath = false;
    currentSceneSourcePathChangedEvent.Invoke(_currentSceneSourcePath);
}

}  // namespace vox
