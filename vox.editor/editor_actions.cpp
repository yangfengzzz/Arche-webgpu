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
#include "vox.render/lua/script_interpreter.h"
#include "vox.render/scene_manager.h"
#include "vox.render/ui/ui_manager.h"

namespace vox {
editor::EditorActions* editor::EditorActions::getSingletonPtr() { return ms_singleton; }

editor::EditorActions& editor::EditorActions::getSingleton() {
    assert(msSingleton);
    return (*ms_singleton);
}

namespace editor {
EditorActions::EditorActions(EditorApplication& app) : app_(app) {}

// MARK: - TOOLS
ui::PanelsManager& EditorActions::panelsManager() { return app_._panelsManager; }

// MARK: - SETTINGS
void EditorActions::setEntitySpawnAtOrigin(bool value) {
    if (value)
        entity_spawn_mode_ = EntitySpawnMode::ORIGIN;
    else
        entity_spawn_mode_ = EntitySpawnMode::FRONT;
}

void EditorActions::setEntitySpawnMode(EntitySpawnMode value) { entity_spawn_mode_ = value; }

void EditorActions::resetLayout() {
    DelayAction([]() { UiManager::GetSingleton().ResetLayout("Config\\layout.ini"); });
}

void EditorActions::setSceneViewCameraSpeed(int speed) {
    auto orbit_control = app_._panelsManager.getPanelAs<ui::SceneView>("Scene View").cameraControl();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::sceneViewCameraSpeed() {
    return app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl()->rotate_speed_;
}

void EditorActions::setAssetViewCameraSpeed(int speed) {
    auto orbit_control = app_._panelsManager.getPanelAs<ui::AssetView>("Asset View").cameraControl();
    orbit_control->rotate_speed_ = speed;
    orbit_control->zoom_speed_ = speed;
}

int EditorActions::assetViewCameraSpeed() {
    return app_.panels_manager_.GetPanelAs<ui::AssetView>("Asset View").CameraControl()->rotate_speed_;
}

void EditorActions::resetSceneViewCameraPosition() {
    auto orbit_control = app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl();
    orbit_control->GetEntity()->transform->SetPosition({-10.0f, 4.0f, 10.0f});
}

void EditorActions::resetAssetViewCameraPosition() {
    auto orbit_control = app_.panels_manager_.GetPanelAs<ui::AssetView>("Asset View").CameraControl();
    orbit_control->GetEntity()->transform->SetPosition({-10.0f, 4.0f, 10.0f});
}

// MARK: - GAME
EditorActions::EditorMode EditorActions::currentEditorMode() const { return _editorMode; }

void EditorActions::setEditorMode(EditorMode newEditorMode) {
    editor_mode_ = new_editor_mode;
    editor_mode_changed_event_.Invoke(editor_mode_);
}

void EditorActions::startPlaying() {
    if (editor_mode_ == EditorMode::EDIT) {
        ScriptInterpreter::GetSingleton().RefreshAll();
        app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").Refresh();

        if (ScriptInterpreter::GetSingleton().IsOk()) {
            play_event_.Invoke();
            scene_backup_.clear();

            nlohmann::json root;
            SceneManager::GetSingleton().CurrentScene()->OnSerialize(root);
            scene_backup_.insert(scene_backup_.begin(), {"root", root});

            app_.panels_manager_.GetPanelAs<ui::GameView>("Game View").Focus();
            SceneManager::GetSingleton().CurrentScene()->Play();
            SetEditorMode(EditorMode::PLAY);
        }
    } else {
        // m_context.audioEngine->Unsuspend();
        SetEditorMode(EditorMode::PLAY);
    }
}

void EditorActions::pauseGame() { SetEditorMode(EditorMode::PAUSE); }

void EditorActions::stopPlaying() {
    if (editor_mode_ != EditorMode::EDIT) {
        // ImGui::GetIO().DisableMouseUpdate = false;
        // m_context.window->SetCursorMode(OvWindowing::Cursor::ECursorMode::NORMAL);
        SetEditorMode(EditorMode::EDIT);
        bool loaded_from_disk = SceneManager::GetSingleton().IsCurrentSceneLoadedFromDisk();
        std::string scene_source_path = SceneManager::GetSingleton().CurrentSceneSourcePath();

        std::string focused_actor_id;

        if (auto target_actor = app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").TargetEntity())
            focused_actor_id = target_actor->name;

        SceneManager::GetSingleton().LoadSceneFromMemory(scene_backup_);
        if (loaded_from_disk) {
            // To bo able to save or reload the scene whereas the scene is loaded from memory (Supposed to have no path)
            SceneManager::GetSingleton().StoreCurrentSceneSourcePath(scene_source_path);
        }
        scene_backup_.clear();
        app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").Focus();
        if (auto actor_instance = SceneManager::GetSingleton().CurrentScene()->FindEntityByName(focused_actor_id)) {
            app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").FocusEntity(actor_instance);
        }
    }
}

void EditorActions::nextFrame() {
    if (editor_mode_ == EditorMode::PLAY || editor_mode_ == EditorMode::PAUSE)
        SetEditorMode(EditorMode::FRAME_BY_FRAME);
}

// MARK: - Entity_CREATION_DESTRUCTION
Vector3F EditorActions::calculateEntitySpawnPoint(float p_distanceToCamera) {
    auto camera_entity = app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").CameraControl()->GetEntity();
    return camera_entity->transform->WorldPosition() + camera_entity->transform->WorldRotationQuaternion() *
                                                               camera_entity->transform->WorldForward() *
                                                               distance_to_camera;
}

Entity* EditorActions::createEmptyEntity(bool p_focusOnCreation, Entity* p_parent, const std::string& p_name) {
    const auto kCurrentScene = SceneManager::GetSingleton().CurrentScene();
    Entity* entity{nullptr};
    if (parent) {
        entity = parent->CreateChild(name);
    } else {
        entity = kCurrentScene->CreateRootEntity(name);
    }

    if (entity_spawn_mode_ == EntitySpawnMode::FRONT)
        entity->transform->SetWorldPosition(CalculateEntitySpawnPoint(10.0f));

    if (focus_on_creation) SelectEntity(entity);

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
void EditorActions::selectEntity(Entity* p_target) {
    app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").FocusEntity(target);
}

void EditorActions::unselectEntity() { app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").UnFocus(); }

bool EditorActions::isAnyEntitySelected() const {
    return app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").TargetEntity();
}

Entity* EditorActions::getSelectedEntity() const {
    return app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").TargetEntity();
}

void EditorActions::moveToTarget(Entity* p_target) {}

// MARK: - RESOURCE_MANAGEMENT
void EditorActions::compileShaders() {}

void EditorActions::saveMaterials() {}

bool EditorActions::importAsset(const std::string& p_initialDestinationDirectory) { return false; }

bool EditorActions::importAssetAtLocation(const std::string& p_destination) { return false; }

std::string EditorActions::getRealPath(const std::string& p_path) {
    std::string result;

    // The path is an engine path
    if (path[0] == ':') {
        result = app_.engine_assets_path_ + std::string(path.data() + 1, path.data() + path.size());
    } else {
        // The path is a project path
        result = app_.project_assets_path_ + path;
    }

    return result;
}

std::string EditorActions::getResourcePath(const std::string& p_path, bool p_isFromEngine) {
    std::string result = path;

    if (Replace(result, is_from_engine ? app_.engine_assets_path_ : app_.project_assets_path_, "")) {
        if (is_from_engine) result = ':' + result;
    }
    return result;
}

std::string EditorActions::getScriptPath(const std::string& p_path) {
    std::string result = path;
    Replace(result, app_.project_scripts_path_, "");
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
    if (CurrentEditorMode() != EditorMode::EDIT) StopPlaying();

    SceneManager::GetSingleton().LoadEmptyLightedScene();
    LOGI("New scene created")
}

void EditorActions::saveCurrentSceneTo(const std::string& p_path) {
    SceneManager::GetSingleton().StoreCurrentSceneSourcePath(path);
    nlohmann::json root;
    SceneManager::GetSingleton().CurrentScene()->OnSerialize(root);

    nlohmann::json j = {
            {"root", root},
    };
    fs::WriteJson(j, path);
}

void EditorActions::loadSceneFromDisk(const std::string& p_path, bool p_absolute) {
    if (CurrentEditorMode() != EditorMode::EDIT) StopPlaying();

    SceneManager::GetSingleton().LoadScene(path, absolute);
    LOGI("Scene loaded from disk: {}", SceneManager::GetSingleton().CurrentSceneSourcePath())
    app_.panels_manager_.GetPanelAs<ui::SceneView>("Scene View").Focus();
}

bool EditorActions::isCurrentSceneLoadedFromDisk() const {
    return SceneManager::GetSingleton().IsCurrentSceneLoadedFromDisk();
}

void EditorActions::saveSceneChanges() {
    if (IsCurrentSceneLoadedFromDisk()) {
        SaveCurrentSceneTo(SceneManager::GetSingleton().CurrentSceneSourcePath());
        LOGI("Current scene saved to: {}" + SceneManager::GetSingleton().CurrentSceneSourcePath())
    } else {
        SaveAs();
    }
}

void EditorActions::saveAs() {
    // todo
}

// MARK: - SCRIPTING
void EditorActions::refreshScripts() {
    ScriptInterpreter::GetSingleton().RefreshAll();
    app_.panels_manager_.GetPanelAs<ui::Inspector>("Inspector").Refresh();
    if (ScriptInterpreter::GetSingleton().IsOk()) {
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
void EditorActions::delayAction(std::function<void()> p_action, uint32_t p_frames) {
    delayed_actions_.emplace_back(frames + 1, action);
}

void EditorActions::executeDelayedActions() {
    std::for_each(delayed_actions_.begin(), delayed_actions_.end(),
                  [](std::pair<uint32_t, std::function<void()>>& element) {
                      --element.first;

                      if (element.first == 0) element.second();
                  });

    delayed_actions_.erase(
            std::remove_if(delayed_actions_.begin(), delayed_actions_.end(),
                           [](std::pair<uint32_t, std::function<void()>>& element) { return element.first == 0; }),
            delayed_actions_.end());
}

}  // namespace editor
}  // namespace vox
