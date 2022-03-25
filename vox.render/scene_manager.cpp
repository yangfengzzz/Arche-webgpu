//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "scene_manager.h"

namespace vox {
SceneManager::SceneManager(wgpu::Device &device, const std::string &p_sceneRootFolder) :
_device(device),
_sceneRootFolder(p_sceneRootFolder) {
    loadEmptyScene();
}

SceneManager::~SceneManager() {
    unloadCurrentScene();
}

void SceneManager::update() {
    if (_delayedLoadCall) {
        _delayedLoadCall();
        _delayedLoadCall = 0;
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
    
    sceneLoadEvent.invoke();
}

bool SceneManager::loadScene(const std::string &p_path, bool p_absolute) {
    std::string completePath = (p_absolute ? "" : _sceneRootFolder) + p_path;
    
    tinyxml2::XMLDocument doc;
    doc.LoadFile(completePath.c_str());
    
    if (loadSceneFromMemory(doc)) {
        storeCurrentSceneSourcePath(completePath);
        return true;
    }
    
    return false;
}

bool SceneManager::loadSceneFromMemory(tinyxml2::XMLDocument &p_doc) {
    if (!p_doc.Error()) {
        tinyxml2::XMLNode *root = p_doc.FirstChild();
        if (root) {
            tinyxml2::XMLNode *sceneNode = root->FirstChildElement("scene");
            if (sceneNode) {
                loadEmptyScene();
                _currentScene->onDeserialize(p_doc, sceneNode);
                return true;
            }
        }
    }
    
    return false;
}

void SceneManager::unloadCurrentScene() {
    if (_currentScene) {
        _currentScene.reset();
        _currentScene = nullptr;
        sceneUnloadEvent.invoke();
    }
    
    forgetCurrentSceneSourcePath();
}

bool SceneManager::hasCurrentScene() const {
    return _currentScene != nullptr;
}

Scene *SceneManager::currentScene() {
    return _currentScene.get();
}

std::string SceneManager::currentSceneSourcePath() const {
    return _currentSceneSourcePath;
}

bool SceneManager::isCurrentSceneLoadedFromDisk() const {
    return _currentSceneLoadedFromPath;
}

void SceneManager::storeCurrentSceneSourcePath(const std::string &p_path) {
    _currentSceneSourcePath = p_path;
    _currentSceneLoadedFromPath = true;
    currentSceneSourcePathChangedEvent.invoke(_currentSceneSourcePath);
}

void SceneManager::forgetCurrentSceneSourcePath() {
    _currentSceneSourcePath = "";
    _currentSceneLoadedFromPath = false;
    currentSceneSourcePathChangedEvent.invoke(_currentSceneSourcePath);
}

}
