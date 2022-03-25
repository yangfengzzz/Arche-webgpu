//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "editor_actions.h"

namespace vox {
editor::EditorActions *editor::EditorActions::getSingletonPtr(void) {
    return msSingleton;
}

editor::EditorActions &editor::EditorActions::getSingleton(void) {
    assert(msSingleton);
    return (*msSingleton);
}

namespace editor {
EditorActions::EditorActions(ui::PanelsManager& p_panelsManager):
_panelsManager(p_panelsManager){
    
}

//MARK: - TOOLS
ui::PanelsManager& EditorActions::panelsManager() {
    return _panelsManager;
}

//MARK: - SETTINGS
void EditorActions::setEntitySpawnAtOrigin(bool value) {
    
}

void EditorActions::setEntitySpawnMode(EntitySpawnMode value) {
    
}

void EditorActions::resetLayout() {
    
}

void EditorActions::setSceneViewCameraSpeed(int speed) {
    
}

int EditorActions::sceneViewCameraSpeed() {
    return 0;
}

void EditorActions::setAssetViewCameraSpeed(int p_speed) {
    
}

int EditorActions::assetViewCameraSpeed() {
    return 0;
}

void EditorActions::resetSceneViewCameraPosition() {
    
}

void EditorActions::resetAssetViewCameraPosition() {
    
}

//MARK: - GAME
EditorActions::EditorMode EditorActions::currentEditorMode() const {
    return _editorMode;
}

void EditorActions::setEditorMode(EditorMode newEditorMode) {
    
}

void EditorActions::startPlaying() {
    
}

void EditorActions::pauseGame() {
    
}

void EditorActions::stopPlaying() {
    
}

void EditorActions::nextFrame() {
    
}

//MARK: - Entity_CREATION_DESTRUCTION
Vector3F EditorActions::calculateEntitySpawnPoint(float p_distanceToCamera) {
    return Vector3F();
}

Entity* EditorActions::createEmptyEntity(bool p_focusOnCreation, Entity* p_parent, const std::string& p_name) {
    return nullptr;
}


Entity* EditorActions::createEntityWithModel(const std::string& p_path, bool p_focusOnCreation,
                                              Entity* p_parent, const std::string& p_name) {
    return nullptr;
}

bool EditorActions::destroyEntity(Entity* entity) {
    return false;
}

void EditorActions::duplicateEntity(Entity* p_toDuplicate, Entity* p_forcedParent, bool p_focus) {
    
}

//MARK: - ENTITY_MANIPULATION
void EditorActions::selectEntity(Entity* p_target) {
    
}

void EditorActions::unselectEntity() {
    
}

bool EditorActions::isAnyEntitySelected() const {
    return false;
}

Entity* EditorActions::getSelectedEntity() const {
    return nullptr;
}

void EditorActions::moveToTarget(Entity* p_target) {
    
}

//MARK: - RESOURCE_MANAGEMENT
void EditorActions::compileShaders() {
    
}

void EditorActions::saveMaterials() {
    
}

bool EditorActions::importAsset(const std::string& p_initialDestinationDirectory) {
    return false;
}

bool EditorActions::importAssetAtLocation(const std::string& p_destination) {
    return false;
}

std::string EditorActions::getRealPath(const std::string& p_path) {
    return "";
}

std::string EditorActions::getResourcePath(const std::string& p_path, bool p_isFromEngine) {
    return "";
}

std::string EditorActions::getScriptPath(const std::string& p_path) {
    return "";
}

void EditorActions::propagateFolderRename(std::string p_previousName, const std::string p_newName) {
    
}

void EditorActions::propagateFolderDestruction(std::string p_folderPath) {
    
}

void EditorActions::propagateScriptRename(std::string p_previousName, std::string p_newName) {
    
}

void EditorActions::propagateFileRename(std::string p_previousName, std::string p_newName) {
    
}

void EditorActions::propagateFileRenameThroughSavedFilesOfType(const std::string& p_previousName, const std::string& p_newName,
                                                               fs::FileType p_fileType) {
    
}


//MARK: - SCENE
void EditorActions::loadEmptyScene() {
    
}

void EditorActions::saveCurrentSceneTo(const std::string& p_path) {
    
}

void EditorActions::loadSceneFromDisk(const std::string& p_path, bool p_absolute) {
    
}

bool EditorActions::isCurrentSceneLoadedFromDisk() const {
    return false;
}

void EditorActions::saveSceneChanges() {
    
}

void EditorActions::saveAs() {
    
}

//MARK: - SCRIPTING
void EditorActions::refreshScripts() {
    
}

//MARK: - BUILDING
std::optional<std::string> EditorActions::selectBuildFolder() {
    return std::nullopt;
}

void EditorActions::build(bool p_autoRun, bool p_tempFolder) {
    
}

void EditorActions::buildAtLocation(const std::string& p_configuration, const std::string p_buildPath, bool p_autoRun) {
    
}

//MARK: - ACTION_SYSTEM
void EditorActions::delayAction(std::function<void()> p_action, uint32_t p_frames) {
    
}

void EditorActions::executeDelayedActions() {
    
}

}
}
