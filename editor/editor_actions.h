//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_actions_hpp
#define editor_actions_hpp

#include "panels_manager.h"
#include "vector3.h"
#include "filesystem.h"
#include "scene_forward.h"
#include "singleton.h"
#include <tinyxml2.h>

namespace vox {
namespace editor {
/**
 * A set of editor actions
 */
class EditorActions : public Singleton<EditorActions> {
public:
    static EditorActions &getSingleton(void);
    
    static EditorActions *getSingletonPtr(void);
    
    /**
     * Constructor
     */
    EditorActions(ui::PanelsManager& p_panelsManager);
    
    //MARK: - TOOLS
public:
    /**
     * Returns the panels manager
     */
    ui::PanelsManager& panelsManager();
    
    //MARK: - SETTINGS
public:
    /**
     * A simple enumeration that define two entity spawn modes
     */
    enum class EntitySpawnMode { ORIGIN, FRONT };
    
    /**
     * Defines if new entities should be spawned at origin
     */
    void setEntitySpawnAtOrigin(bool value);
    
    /**
     * Defines how new entities should be spawned
     */
    void setEntitySpawnMode(EntitySpawnMode value);
    
    /**
     * Reset the editor layout
     */
    void resetLayout();
    
    /**
     * Defines the scene view camera speed
     */
    void setSceneViewCameraSpeed(int speed);
    
    /**
     * Returns the scene view camera speed
     */
    int sceneViewCameraSpeed();
    
    /**
     * Defines the asset view camera speed
     */
    void setAssetViewCameraSpeed(int p_speed);
    
    /**
     * Returns the asset view camera speed
     */
    int assetViewCameraSpeed();
    
    /**
     * Resets the scene view camera position to the default one
     */
    void resetSceneViewCameraPosition();
    
    /**
     * Resets the scene view camera position to the default one
     */
    void resetAssetViewCameraPosition();
    
    //MARK: - GAME
public:
    /**
     * Defines some states for the editor
     */
    enum class EditorMode { EDIT, PLAY, PAUSE, FRAME_BY_FRAME };
    
    /**
     * Returns the current editor state/mode
     */
    EditorMode currentEditorMode() const;
    
    /**
     * Defines the editor state/mode
     */
    void setEditorMode(EditorMode newEditorMode);
    
    /**
     * Start playing the current scene and update the editor mode
     */
    void startPlaying();
    
    /**
     * Pause the current playing scene and update the editor mode
     */
    void pauseGame();
    
    /**
     * Stop platying the current scene and update the editor mode
     */
    void stopPlaying();
    
    /**
     * Play the current frame and pause the editor
     */
    void nextFrame();
    
    //MARK: - Entity_CREATION_DESTRUCTION
public:
    /**
     * Create an entity with the given component type
     */
    template<typename T>
    Entity* createMonoComponentEntity(bool p_focusOnCreation = true, Entity* p_parent = nullptr);
    
    /**
     * Calculate the position where to spawn the entity using the current camera position and forward
     */
    Vector3F calculateEntitySpawnPoint(float p_distanceToCamera);
    
    /**
     * Create an empty entity
     */
    Entity* createEmptyEntity(bool p_focusOnCreation = true, Entity* p_parent = nullptr,
                                const std::string& p_name = "");
    
    /**
     * Create an entity with a model renderer and a material renderer. The model renderer with use the model identified
     * by the given path
     */
    Entity* createEntityWithModel(const std::string& p_path, bool p_focusOnCreation = true,
                                   Entity* p_parent = nullptr, const std::string& p_name = "");
    
    /**
     * Destroy an entity from his scene
     */
    bool destroyEntity(Entity* p_entity);
    
    /**
     * Duplicate an entity
     */
    void duplicateEntity(Entity* p_toDuplicate, Entity* p_forcedParent = nullptr, bool p_focus = true);
    
    
    //MARK: - ENTITY_MANIPULATION
public:
    /**
     * Select an entity and show him in inspector
     */
    void selectEntity(Entity* p_target);
    
    /**
     * Unselect any selected entity and clearing the inspector
     */
    void unselectEntity();
    
    /**
     * Returns true if any entity is selected
     */
    bool isAnyEntitySelected() const;
    
    /**
     * Returns the selected entity. Make sur you verified that an entity is selected
     * with IsAnyEntitySelected() before calling this method
     */
    Entity* getSelectedEntity() const;
    
    /**
     * Moves the camera to the target entity
     */
    void moveToTarget(Entity* p_target);
    
    //MARK: - RESOURCE_MANAGEMENT
public:
    /**
     * Compile every loaded shaders
     */
    void compileShaders();
    
    /**
     * Save every materials to their respective files
     */
    void saveMaterials();
    
    /**
     * Import an asset
     */
    bool importAsset(const std::string& p_initialDestinationDirectory);
    
    /**
     * Import an asset at location
     */
    bool importAssetAtLocation(const std::string& p_destination);
    
    /**
     * Returns the real path of a resource (complete absolute path)
     */
    std::string getRealPath(const std::string& p_path);
    
    /**
     * Returns the resource path of a file
     */
    std::string getResourcePath(const std::string& p_path, bool p_isFromEngine = false);
    
    /**
     * Returns the script path of a file
     */
    std::string getScriptPath(const std::string& p_path);
    
    /**
     * Propagate the folder rename everywhere (Resource manager, scenes, materials...)
     */
    void propagateFolderRename(std::string p_previousName, const std::string p_newName);
    
    /**
     * Propagate the folder destruction everywhere (Resource manager, scenes, materials...)
     */
    void propagateFolderDestruction(std::string p_folderPath);
    
    /**
     * Propagate the script rename in scenes and inspector
     */
    void propagateScriptRename(std::string p_previousName, std::string p_newName);
    
    /**
     * Propagate the file rename everywhere it is used
     */
    void propagateFileRename(std::string p_previousName, std::string p_newName);
    
    /**
     * Propagate the file rename through concerned files
     */
    void propagateFileRenameThroughSavedFilesOfType(const std::string& p_previousName, const std::string& p_newName,
                                                    fs::FileType p_fileType);
    
    
    //MARK: - SCENE
public:
    /**
     * Load an empty scene. Any unsaved changes to the current scene will be discarded
     */
    void loadEmptyScene();
    
    /**
     * Save the current scene to the given path
     */
    void saveCurrentSceneTo(const std::string& p_path);
    
    /**
     * Load a scene from the disk
     */
    void loadSceneFromDisk(const std::string& p_path, bool p_absolute = false);
    
    /**
     * Returns true if the current scene has been loaded from disk
     */
    bool isCurrentSceneLoadedFromDisk() const;
    
    /**
     * Save the current scene to its disk location
     */
    void saveSceneChanges();
    
    /**
     * Save the current scene to a new disk location (Can create a duplication of the scene file)
     */
    void saveAs();
    
    //MARK: - SCRIPTING
public:
    /**
     * Refresh every scripts (Re-interpret)
     */
    void refreshScripts();
    
    
    //MARK: - BUILDING
public:
    /**
     * Ask the user to select the build folder
     */
    std::optional<std::string> selectBuildFolder();
    
    /**
     * Build the current project
     */
    void build(bool p_autoRun = false, bool p_tempFolder = false);
    
    /**
     * Build the current project at the given location
     */
    void buildAtLocation(const std::string& p_configuration, const std::string p_buildPath, bool p_autoRun = false);
    
    //MARK: - ACTION_SYSTEM
    /**
     * Prepare an action for a future call
     */
    void delayAction(std::function<void()> p_action, uint32_t p_frames = 1);
    
    /**
     * Execute every actions that should be executed at this frame (Decrement the frame counter for each actions)
     */
    void executeDelayedActions();
    
public:
    Event<Entity*> entitySelectedEvent;
    Event<Entity*> entityUnselectedEvent;
    Event<EditorMode> editorModeChangedEvent;
    Event<> playEvent;
    
private:
    ui::PanelsManager& _panelsManager;
    
    EntitySpawnMode _entitySpawnMode = EntitySpawnMode::ORIGIN;
    EditorMode _editorMode = EditorMode::EDIT;
    
    std::vector<std::pair<uint32_t, std::function<void()>>> _delayedActions;
    
    tinyxml2::XMLDocument _sceneBackup;
};

}
template<> inline editor::EditorActions* Singleton<editor::EditorActions>::msSingleton{nullptr};
}
#include "editor_actions-inl.h"

#endif /* editor_actions_hpp */
