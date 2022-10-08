//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/script.h"

#include "vox.render/components_manager.h"
#include "vox.render/entity.h"
#include "vox.render/physics/physics_manager.h"
#include "vox.render/scene.h"

namespace vox {
std::string Script::name() { return "Script"; }

Script::Script(Entity *entity) : Component(entity) {}

Script::~Script() { ComponentsManager::getSingleton().addDestroyComponent(this); }

void Script::setIsStarted(bool value) { _started = value; }

bool Script::isStarted() const { return _started; }

void Script::_onAwake() { onAwake(); }

void Script::_onEnable() {
    auto componentsManager = ComponentsManager::getSingletonPtr();
    if (!_started) {
        componentsManager->addOnStartScript(this);
    }
    componentsManager->addOnUpdateScript(this);
    PhysicsManager::GetSingleton().addOnPhysicsUpdateScript(this);
    _entity->_addScript(this);
    onEnable();
}

void Script::_onDisable() {
    auto componentsManager = ComponentsManager::getSingletonPtr();
    componentsManager->removeOnStartScript(this);
    componentsManager->removeOnUpdateScript(this);
    PhysicsManager::GetSingleton().removeOnPhysicsUpdateScript(this);
    _entity->_removeScript(this);
    onDisable();
}

// MARK: - Reflection
void Script::onSerialize(nlohmann::json &data) {}

void Script::onDeserialize(nlohmann::json &data) {}

void Script::onInspector(ui::WidgetContainer &p_root) {}

}  // namespace vox
