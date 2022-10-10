//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/editor_actions.h"

#include "vox.editor/editor_application.h"
#include "vox.editor/ui/inspector.h"
#include "vox.editor/view/asset_view.h"
#include "vox.editor/view/game_view.h"
#include "vox.editor/view/scene_view.h"
#include "vox.render/strings.h"

namespace vox {
editor::EditorActions* editor::EditorActions::getSingletonPtr() { return ms_singleton; }

editor::EditorActions& editor::EditorActions::getSingleton() {
    assert(ms_singleton);
    return (*ms_singleton);
}

namespace editor {
EditorActions::EditorActions(EditorApplication& app) : app_(app) {}

// MARK: - TOOLS
ui::PanelsManager& EditorActions::panelsManager() { return app_._panelsManager; }

// MARK: - SETTINGS
void EditorActions::setEntitySpawnAtOrigin(bool value) {
    if (value)
        _entitySpawnMode = EntitySpawnMode::ORIGIN;
    else
        _entitySpawnMode = EntitySpawnMode::FRONT;
}

void EditorActions::setEntitySpawnMode(EntitySpawnMode value) { _entitySpawnMode = value; }

void EditorActions::resetLayout() {
    delayAction([]() { UiManager::GetSingleton().ResetLayout("Config\\layout.ini"); });
}

void EditorActions::setSceneViewCameraSpeed(int speed) {
    auto orbit_control = app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").cameraControl();
    orbit_control->rotateSpeed = speed;
    orbit_control->zoomSpeed = speed;
}

int EditorActions::sceneViewCameraSpeed() {
    return app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").cameraControl()->rotateSpeed;
}

void EditorActions::setAssetViewCameraSpeed(int speed) {
    auto orbit_control = app_._panelsManager.getPanelAs<ui::AssetView>("Asset View").cameraControl();
    orbit_control->rotateSpeed = speed;
    orbit_control->zoomSpeed = speed;
}

int EditorActions::assetViewCameraSpeed() {
    return app_._panelsManager.getPanelAs<ui::AssetView>("Asset View").cameraControl()->rotateSpeed;
}

void EditorActions::resetSceneViewCameraPosition() {
    auto orbit_control = app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").cameraControl();
    orbit_control->entity()->transform->setPosition({-10.0f, 4.0f, 10.0f});
}

void EditorActions::resetAssetViewCameraPosition() {
    auto orbit_control = app_._panelsManager.getPanelAs<ui::AssetView>("Asset View").cameraControl();
    orbit_control->entity()->transform->setPosition({-10.0f, 4.0f, 10.0f});
}

// MARK: - GAME
EditorActions::EditorMode EditorActions::currentEditorMode() const { return _editorMode; }

void EditorActions::setEditorMode(EditorMode newEditorMode) {
    _editorMode = newEditorMode;
    editorModeChangedEvent.Invoke(_editorMode);
}

void EditorActions::startPlaying() {
    if (_editorMode == EditorMode::EDIT) {
        ScriptInterpreter::GetSingleton().refreshAll();
        app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").refresh();

        if (ScriptInterpreter::GetSingleton().isOk()) {
            playEvent.Invoke();
            scene_backup_.clear();

            nlohmann::json root;
            SceneManager::GetSingleton().currentScene()->onSerialize(root);
            scene_backup_.insert(scene_backup_.begin(), {"root", root});

            app_._panelsManager.getPanelAs<ui::GameView>("Game View").Focus();
            SceneManager::GetSingleton().currentScene()->play();
            setEditorMode(EditorMode::PLAY);
        }
    } else {
        // m_context.audioEngine->Unsuspend();
        setEditorMode(EditorMode::PLAY);
    }
}

void EditorActions::pauseGame() { setEditorMode(EditorMode::PAUSE); }

void EditorActions::stopPlaying() {
    if (_editorMode != EditorMode::EDIT) {
        // ImGui::GetIO().DisableMouseUpdate = false;
        // m_context.window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
        setEditorMode(EditorMode::EDIT);
        bool loaded_from_disk = SceneManager::GetSingleton().isCurrentSceneLoadedFromDisk();
        std::string scene_source_path = SceneManager::GetSingleton().currentSceneSourcePath();

        std::string focused_actor_id;

        if (auto target_actor = app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").targetEntity())
            focused_actor_id = target_actor->name;

        SceneManager::GetSingleton().loadSceneFromMemory(scene_backup_);
        if (loaded_from_disk) {
            // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
            SceneManager::GetSingleton().storeCurrentSceneSourcePath(scene_source_path);
        }
        scene_backup_.clear();
        app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").Focus();
        if (auto actor_instance = SceneManager::GetSingleton().currentScene()->findEntityByName(focused_actor_id)) {
            app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").focusEntity(actor_instance);
        }
    }
}

void EditorActions::nextFrame() {
    if (_editorMode == EditorMode::PLAY || _editorMode == EditorMode::PAUSE) setEditorMode(EditorMode::FRAME_BY_FRAME);
}

// MARK: - Entity_CREATION_DESTRUCTION
Point3F EditorActions::calculateEntitySpawnPoint(float p_distanceToCamera) {
    auto camera_entity = app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").cameraControl()->entity();
    return camera_entity->transform->worldPosition() + camera_entity->transform->worldRotationQuaternion() *
                                                               camera_entity->transform->worldForward() *
                                                               p_distanceToCamera;
}

Entity* EditorActions::createEmptyEntity(bool focusOnCreation, Entity* parent, const std::string& name) {
    const auto kCurrentScene = SceneManager::GetSingleton().currentScene();
    Entity* entity{nullptr};
    if (parent) {
        entity = parent->createChild(name);
    } else {
        entity = kCurrentScene->createRootEntity(name);
    }

    if (_entitySpawnMode == EntitySpawnMode::FRONT)
        entity->transform->setWorldPosition(calculateEntitySpawnPoint(10.0f));

    if (focusOnCreation) selectEntity(entity);

    LOGI("Entity created")

    return entity;
}

Entity* EditorActions::createEntityWithModel(const std::string& p_path,
                                             bool p_focusOnCreation,
                                             Entity* p_parent,
                                             const std::string& p_name) {
    return nullptr;
}

bool EditorActions::destroyEntity(Entity* entity) {
    // entity.MarkAsDestroy();
    LOGI("Entity destroyed")
    return true;
}

void EditorActions::duplicateEntity(Entity* p_toDuplicate, Entity* p_forcedParent, bool p_focus) {}

// MARK: - ENTITY_MANIPULATION
void EditorActions::selectEntity(Entity* target) {
    app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").focusEntity(target);
}

void EditorActions::unselectEntity() { app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").unFocus(); }

bool EditorActions::isAnyEntitySelected() const {
    return app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").targetEntity();
}

Entity* EditorActions::getSelectedEntity() const {
    return app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").targetEntity();
}

void EditorActions::moveToTarget(Entity* p_target) {}

// MARK: - RESOURCE_MANAGEMENT
void EditorActions::compileShaders() {}

void EditorActions::saveMaterials() {}

bool EditorActions::importAsset(const std::string& p_initialDestinationDirectory) { return false; }

bool EditorActions::importAssetAtLocation(const std::string& p_destination) { return false; }

std::string EditorActions::getRealPath(const std::string& path) {
    std::string result;

    // The path is an engine path
    if (path[0] == ':') {
        result = app_.engineAssetsPath + std::string(path.data() + 1, path.data() + path.size());
    } else {
        // The path is a project path
        result = app_.projectAssetsPath + path;
    }

    return result;
}

std::string EditorActions::getResourcePath(const std::string& path, bool isFromEngine) {
    std::string result = path;

    if (Replace(result, isFromEngine ? app_.engineAssetsPath : app_.projectAssetsPath, "")) {
        if (isFromEngine) result = ':' + result;
    }
    return result;
}

std::string EditorActions::getScriptPath(const std::string& path) {
    std::string result = path;
    Replace(result, app_.projectScriptsPath, "");
    Replace(result, ".lua", "");
    return result;
}

void EditorActions::propagateFolderRename(std::string p_previousName, const std::string p_newName) {}

void EditorActions::propagateFolderDestruction(std::string p_folderPath) {}

void EditorActions::propagateScriptRename(std::string p_previousName, std::string p_newName) {}

void EditorActions::propagateFileRename(std::string p_previousName, std::string p_newName) {}

void EditorActions::propagateFileRenameThroughSavedFilesOfType(const std::string& p_previousName,
                                                               const std::string& p_newName,
                                                               fs::FileType p_fileType) {}

// MARK: - SCENE
void EditorActions::loadEmptyScene() {
    if (currentEditorMode() != EditorMode::EDIT) stopPlaying();

    SceneManager::GetSingleton().loadEmptyLightedScene();
    LOGI("New scene created")
}

void EditorActions::saveCurrentSceneTo(const std::string& path) {
    SceneManager::GetSingleton().storeCurrentSceneSourcePath(path);
    nlohmann::json root;
    SceneManager::GetSingleton().currentScene()->onSerialize(root);

    nlohmann::json j = {
            {"root", root},
    };
    fs::WriteJson(j, path);
}

void EditorActions::loadSceneFromDisk(const std::string& path, bool absolute) {
    if (currentEditorMode() != EditorMode::EDIT) stopPlaying();

    SceneManager::GetSingleton().loadScene(path, absolute);
    LOGI("Scene loaded from disk: {}", SceneManager::GetSingleton().currentSceneSourcePath())
    app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").Focus();
}

bool EditorActions::isCurrentSceneLoadedFromDisk() const {
    return SceneManager::GetSingleton().isCurrentSceneLoadedFromDisk();
}

void EditorActions::saveSceneChanges() {
    if (isCurrentSceneLoadedFromDisk()) {
        saveCurrentSceneTo(SceneManager::GetSingleton().currentSceneSourcePath());
        LOGI("Current scene saved to: {}" + SceneManager::GetSingleton().currentSceneSourcePath())
    } else {
        saveAs();
    }
}

void EditorActions::saveAs() {
    // todo
}

// MARK: - SCRIPTING
void EditorActions::refreshScripts() {
    ScriptInterpreter::GetSingleton().refreshAll();
    app_._panelsManager.getPanelAs<ui::Inspector>("Inspector").refresh();
    if (ScriptInterpreter::GetSingleton().isOk()) {
        LOGI("Scripts interpretation succeeded!")
    }
}

// MARK: - BUILDING
std::optional<std::string> EditorActions::selectBuildFolder() {
    // todo
    return std::nullopt;
}

void EditorActions::build(bool p_autoRun, bool p_tempFolder) {
    // todo
}

void EditorActions::buildAtLocation(const std::string& p_configuration, const std::string p_buildPath, bool p_autoRun) {
    // todo
}

// MARK: - ACTION_SYSTEM
void EditorActions::delayAction(std::function<void()> action, uint32_t frames) {
    _delayedActions.emplace_back(frames + 1, action);
}

void EditorActions::executeDelayedActions() {
    std::for_each(_delayedActions.begin(), _delayedActions.end(),
                  [](std::pair<uint32_t, std::function<void()>>& element) {
                      --element.first;

                      if (element.first == 0) element.second();
                  });

    _delayedActions.erase(
            std::remove_if(_delayedActions.begin(), _delayedActions.end(),
                           [](std::pair<uint32_t, std::function<void()>>& element) { return element.first == 0; }),
            _delayedActions.end());
}

}  // namespace editor
}  // namespace vox
