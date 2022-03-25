//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script.h"
#include "entity.h"
#include "scene.h"
#include "components_manager.h"

namespace vox {
std::string Script::name() {
    return "Script";
}

Script::Script(Entity *entity) :
Component(entity) {
    
}

Script::~Script() {
    ComponentsManager::getSingleton().addDestroyComponent(this);
}

void Script::setIsStarted(bool value) {
    _started = value;
}

bool Script::isStarted() {
    return _started;
}

void Script::_onAwake() {
    onAwake();
}

void Script::_onEnable() {
    auto componentsManager = ComponentsManager::getSingletonPtr();
    if (!_started) {
        componentsManager->addOnStartScript(this);
    }
    componentsManager->addOnUpdateScript(this);
    _entity->_addScript(this);
    onEnable();
}

void Script::_onDisable() {
    auto componentsManager = ComponentsManager::getSingletonPtr();
    componentsManager->removeOnStartScript(this);
    componentsManager->removeOnUpdateScript(this);
    _entity->_removeScript(this);
    onDisable();
}

//MARK: - Reflection
void Script::onSerialize(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node) {
    
}

void Script::onDeserialize(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_node) {
    
}

void Script::onInspector(ui::WidgetContainer &p_root) {
    
}

}
