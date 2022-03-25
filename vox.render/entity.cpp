//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "entity.h"
#include "scene.h"

#include "component.h"
#include "script.h"
#include "serializer.h"
#include <glog/logging.h>

namespace vox {
Event<Entity *> Entity::destroyedEvent;
Event<Entity *> Entity::createdEvent;
Event<Entity *, Entity *> Entity::attachEvent;
Event<Entity *> Entity::dettachEvent;

Entity *Entity::_findChildByName(Entity *root, const std::string &name) {
    const auto &children = root->_children;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        if (child->name == name) {
            return child.get();
        }
    }
    return nullptr;
}

void Entity::_traverseSetOwnerScene(Entity *entity, Scene *scene) {
    entity->_scene = scene;
    const auto &children = entity->_children;
    for (size_t i = 0; i < entity->_children.size(); i++) {
        _traverseSetOwnerScene(children[i].get(), scene);
    }
}

Entity::Entity(std::string name) : name(name) {
    transform = addComponent<Transform>();
    _inverseWorldMatFlag = transform->registerWorldChangeFlag();
    
    createdEvent.invoke(this);
}

Entity::~Entity() {
    if (_parent) {
        LOG(ERROR) << "use removeChild instead! \n";
    }
    _components.clear();
    _children.clear();
}

bool Entity::isActive() {
    return _isActive;
}

void Entity::setIsActive(bool value) {
    if (value != _isActive) {
        _isActive = value;
        if (value) {
            const auto &parent = _parent;
            if ((parent != nullptr && parent->_isActiveInHierarchy)
                || (_isRoot)) {
                _processActive();
            }
        } else {
            if (_isActiveInHierarchy) {
                _processInActive();
            }
        }
    }
}

bool Entity::isActiveInHierarchy() {
    return _isActiveInHierarchy;
}

Entity *Entity::parent() {
    return _parent;
}

const std::vector<std::unique_ptr<Entity>> &Entity::children() const {
    return _children;
}

size_t Entity::childCount() {
    return _children.size();
}

Scene *Entity::scene() {
    return _scene;
}

void Entity::addChild(std::unique_ptr<Entity> &&child) {
    if (child->_parent != this) {
        child->_removeFromParent();
        child->_parent = this;
        if (child->_scene != _scene) {
            Entity::_traverseSetOwnerScene(child.get(), _scene);
        }
        
        if (_isActiveInHierarchy) {
            if (!child->_isActiveInHierarchy && child->_isActive) {
                child->_processActive();
            }
        } else {
            if (child->_isActiveInHierarchy) {
                child->_processInActive();
            }
        }
        child->_setTransformDirty();
        _children.emplace_back(std::move(child));
    }
}

void Entity::removeChild(Entity *child) {
    if (child->_parent == this) {
        child->_removeFromParent();
        child->_parent = nullptr;
        
        if (child->_isActiveInHierarchy) {
            child->_processInActive();
        }
        Entity::_traverseSetOwnerScene(child, nullptr);
        child->_setTransformDirty();
    }
}

Entity *Entity::getChild(int index) {
    return _children[index].get();
}

Entity *Entity::findByName(const std::string &name) {
    const auto &children = _children;
    const auto child = Entity::_findChildByName(this, name);
    if (child) return child;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        const auto grandson = child->findByName(name);
        if (grandson) {
            return grandson;
        }
    }
    return nullptr;
}

Entity *Entity::createChild(const std::string &name) {
    auto child = std::make_unique<Entity>(name);
    auto childPtr = child.get();
    child->layer = layer;
    addChild(std::move(child));
    return childPtr;
}

void Entity::clearChildren() {
    auto &children = _children;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        child->_parent = nullptr;
        if (child->_isActiveInHierarchy) {
            child->_processInActive();
        }
        Entity::_traverseSetOwnerScene(child.get(), nullptr); // Must after child._processInActive().
    }
    children.clear();
}

void Entity::removeComponent(Component *component) {
    // ComponentsDependencies._removeCheck(this, component.constructor as any);
    _components.erase(std::remove_if(_components.begin(),
                                     _components.end(), [&](const std::unique_ptr<Component> &x) {
        return x.get() == component;
    }), _components.end());
}

std::unique_ptr<Entity> Entity::clone() {
    auto cloneEntity = std::make_unique<Entity>(name);
    
    cloneEntity->_isActive = _isActive;
    cloneEntity->transform->setLocalMatrix(transform->localMatrix());
    
    const auto &children = _children;
    for (size_t i = 0, len = _children.size(); i < len; i++) {
        const auto &child = children[i];
        cloneEntity->addChild(child->clone());
    }
    
    const auto &components = _components;
    for (size_t i = 0, n = components.size(); i < n; i++) {
        const auto &sourceComp = components[i];
        if (!(dynamic_cast<Transform *>(sourceComp.get()))) {
            // TODO
        }
    }
    
    return cloneEntity;
}

void Entity::_addScript(Script *script) {
    script->_entityCacheIndex = _scripts.size();
    _scripts.push_back(script);
}

void Entity::_removeScript(Script *script) {
    _scripts.erase(std::remove(_scripts.begin(), _scripts.end(), script), _scripts.end());
    script->_entityCacheIndex = -1;
}

Entity *Entity::_removeFromParent() {
    auto oldParent = _parent;
    if (oldParent != nullptr) {
        auto &oldParentChildren = oldParent->_children;
        oldParentChildren.erase(std::remove_if(oldParentChildren.begin(), oldParentChildren.end(),
                                               [&](const auto &child) {
            return child.get() == this;
        }), oldParentChildren.end());
        _parent = nullptr;
    }
    return oldParent;
}

void Entity::_processActive() {
    _activeChangedComponents = _scene->_componentsManager.getActiveChangedTempList();
    _setActiveInHierarchy(_activeChangedComponents);
    _setActiveComponents(true);
}

void Entity::_processInActive() {
    _activeChangedComponents = _scene->_componentsManager.getActiveChangedTempList();
    _setInActiveInHierarchy(_activeChangedComponents);
    _setActiveComponents(false);
}

void Entity::_setActiveComponents(bool isActive) {
    auto &activeChangedComponents = _activeChangedComponents;
    for (size_t i = 0, length = activeChangedComponents.size(); i < length; ++i) {
        activeChangedComponents[i]->_setActive(isActive);
    }
    _scene->_componentsManager.putActiveChangedTempList(activeChangedComponents);
    _activeChangedComponents.clear();
}

void Entity::_setActiveInHierarchy(std::vector<Component *> &activeChangedComponents) {
    _isActiveInHierarchy = true;
    auto &components = _components;
    for (size_t i = 0; i < components.size(); i++) {
        activeChangedComponents.push_back(components[i].get());
    }
    const auto &children = _children;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        if (child->isActive()) {
            child->_setActiveInHierarchy(activeChangedComponents);
        }
    }
}

void Entity::_setInActiveInHierarchy(std::vector<Component *> &activeChangedComponents) {
    _isActiveInHierarchy = false;
    auto &components = _components;
    for (size_t i = 0; i < components.size(); i++) {
        activeChangedComponents.push_back(components[i].get());
    }
    auto &children = _children;
    for (size_t i = 0; i < children.size(); i++) {
        const auto &child = children[i];
        if (child->isActive()) {
            child->_setInActiveInHierarchy(activeChangedComponents);
        }
    }
}

void Entity::_setTransformDirty() {
    if (transform) {
        transform->_parentChange();
    } else {
        for (size_t i = 0, len = _children.size(); i < len; i++) {
            _children[i]->_setTransformDirty();
        }
    }
}

std::vector<Script *> Entity::scripts() {
    return _scripts;
}

//MARK: - Reflection
void Entity::onSerialize(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_actorsRoot) {
    tinyxml2::XMLNode *actorNode = p_doc.NewElement("actor");
    p_actorsRoot->InsertEndChild(actorNode);
    
    Serializer::serializeString(p_doc, actorNode, "name", name);
    
    tinyxml2::XMLNode *componentsNode = p_doc.NewElement("components");
    actorNode->InsertEndChild(componentsNode);
    for (auto &component: _components) {
        /* Current component root */
        tinyxml2::XMLNode *componentNode = p_doc.NewElement("component");
        componentsNode->InsertEndChild(componentNode);
        
        /* Component type */
        Serializer::serializeString(p_doc, componentNode, "type", typeid(component).name());
        
        /* Data node (Will be passed to the component) */
        tinyxml2::XMLElement *data = p_doc.NewElement("data");
        componentNode->InsertEndChild(data);
        
        /* Data serialization of the component */
        component->onSerialize(p_doc, data);
    }
}

void Entity::onDeserialize(tinyxml2::XMLDocument &p_doc, tinyxml2::XMLNode *p_actorsRoot) {
    Serializer::deserializeString(p_doc, p_actorsRoot, "name", name);
    
    {
        tinyxml2::XMLNode *componentsRoot = p_actorsRoot->FirstChildElement("components");
        if (componentsRoot) {
            tinyxml2::XMLElement *currentComponent = componentsRoot->FirstChildElement("component");
            
            while (currentComponent) {
                std::string componentType = currentComponent->FirstChildElement("type")->GetText();
                currentComponent = currentComponent->NextSiblingElement("component");
            }
        }
    }
    
    {
        tinyxml2::XMLNode *behavioursRoot = p_actorsRoot->FirstChildElement("behaviours");
        
        if (behavioursRoot) {
            tinyxml2::XMLElement *currentBehaviour = behavioursRoot->FirstChildElement("behaviour");
            
            while (currentBehaviour) {
                std::string behaviourType = currentBehaviour->FirstChildElement("type")->GetText();
            }
        }
    }
}

}        // namespace vox
