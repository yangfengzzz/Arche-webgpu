//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "script.h"
#include "entity.h"
#include "scene.h"

namespace vox {
std::string Script::name() {
    return "Script";
}

Script::Script(Entity *entity) :
Component(entity) {
    
}

void Script::_onAwake() {
    onAwake();
}

void Script::_onEnable() {
    auto &componentsManager = entity()->scene()->_componentsManager;
    if (!_started) {
        componentsManager.addOnStartScript(this);
    }
    componentsManager.addOnUpdateScript(this);
    _entity->_addScript(this);
    onEnable();
}

void Script::_onDisable() {
    auto &componentsManager = entity()->scene()->_componentsManager;
    // Use "xxIndex" is more safe.
    // When call onDisable it maybe it still not in script queue,for example write "entity.isActive = false" in onWake().
    if (_onStartIndex != -1) {
        componentsManager.removeOnStartScript(this);
    }
    if (_onUpdateIndex != -1) {
        componentsManager.removeOnUpdateScript(this);
    }
    if (_entityCacheIndex != -1) {
        _entity->_removeScript(this);
    }
    onDisable();
}

void Script::_onDestroy() {
    entity()->scene()->_componentsManager.addDestroyComponent(this);
}

//MARK: - Reflection
void Script::onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void Script::onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) {
    
}

void Script::onInspector(ui::WidgetContainer& p_root) {
    
}

}
