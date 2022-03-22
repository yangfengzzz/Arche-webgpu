//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef scene_manager_hpp
#define scene_manager_hpp

#include "singleton.h"
#include "scene.h"
#include "event.h"

namespace vox {
/**
 * The scene manager of the current scene
 */
class SceneManager {
public:
    
    /**
     * Default constructor
     * @param p_sceneRootFolder (Optional)
     */
    SceneManager(wgpu::Device &device, const std::string &p_sceneRootFolder = "");
    
    /**
     * Default destructor
     */
    ~SceneManager();
    
    /**
     * Update
     */
    void update();
    
    /**
     * Load an play a scene with a delay
     * @param p_path p_path
     * @param p_absolute p_absolute
     */
    void loadAndPlayDelayed(const std::string &p_path, bool p_absolute = false);
    
    /**
     * Load an empty scene in memory
     */
    void loadEmptyScene();
    
    /**
     * Load specific scene in memory
     * @param p_path p_path
     * @param p_absolute (If this setting is set to true, the scene loader will ignore the "SceneRootFolder" given on SceneManager construction)
     */
    bool loadScene(const std::string &p_path, bool p_absolute = false);
    
    /**
     * Load specific scene in memory
     * @param p_doc p_doc
     */
    bool loadSceneFromMemory(tinyxml2::XMLDocument &p_doc);
    
    /**
     * Destroy current scene from memory
     */
    void unloadCurrentScene();
    
    /**
     * Return true if a scene is currently loaded
     */
    bool hasCurrentScene() const;
    
    /*
     * Return current loaded scene
     */
    Scene *currentScene();
    
    /**
     * Return the current scene source path
     */
    std::string currentSceneSourcePath() const;
    
    /**
     * Return true if the currently loaded scene has been loaded from a file
     */
    bool isCurrentSceneLoadedFromDisk() const;
    
    /**
     * Store the given path as the current scene source path
     * @param p_path p_path
     */
    void storeCurrentSceneSourcePath(const std::string &p_path);
    
    /**
     * Reset the current scene source path to an empty string
     */
    void forgetCurrentSceneSourcePath();
    
public:
    Event<> sceneLoadEvent;
    Event<> sceneUnloadEvent;
    Event<const std::string &> currentSceneSourcePathChangedEvent;
    
private:
    wgpu::Device &_device;
    const std::string _sceneRootFolder;
    std::unique_ptr<Scene> _currentScene{nullptr};
    
    bool _currentSceneLoadedFromPath{false};
    std::string _currentSceneSourcePath = "";
    
    std::function<void()> _delayedLoadCall;
};
}

#endif /* scene_manager_hpp */
